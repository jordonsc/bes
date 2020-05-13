#include "request.h"

using namespace bes::fastcgi;

Request::Request(Transceiver& tns, bes::Container const& cnt) : container(cnt), transceiver(tns) {}

bool Request::Run()
{
    try {
        // Process all input records
        while (ProcessRecord()) {
        }

        // Validate we have valid data
        if (static_cast<uint16_t>(role) == 0) {
            throw PayloadException("No role provided for request");
        }

        return true;

    } catch (AbortException const&) {
        return false;
    }
}

/**
 * Read a header and then process the record-type it contains
 */
bool Request::ProcessRecord()
{
    model::Header header = transceiver.ReadModel<model::Header>();

    // Validate we understand the FCGI version
    if (header.version > model::fcgi_version) {
        throw PayloadException("Request version mismatch: expected: " + std::to_string(model::fcgi_version) +
                               ", received: " + std::to_string(header.version));
    }

    // Validate we're receiving the same request ID for each record
    if (request_id == 0) {
        request_id = header.request_id;
    } else if (request_id != header.request_id) {
        throw PayloadException("Request ID mismatch: expected: " + std::to_string(request_id) +
                               ", received: " + std::to_string(header.request_id));
    }

    // Select the appropriate responder
    switch (header.type) {
        case model::RecordType::BEGIN_REQUEST:
            ProcessBeginRequest(header);
            return true;

        case model::RecordType::ABORT_REQUEST:
            transceiver.SkipRecord(header);
            throw AbortException("Server aborted request");

        case model::RecordType::PARAMS:
            ProcessParams(header);
            return true;

        case model::RecordType::IN:
            ProcessIn(header);
            return false;

        default:
            BES_LOG(WARNING) << "FCGI: server skipping record-type: " << int(static_cast<uint8_t>(header.type));
            transceiver.SkipRecord(header);
            return true;
    }
}

/**
 * The BeginRequest segment marks the intended role and sets some flags.
 */
void Request::ProcessBeginRequest(model::Header const& header)
{
    ValidateRecordLength(header, sizeof(model::BeginRequest));

    model::BeginRequest begin_request = transceiver.ReadModel<model::BeginRequest>();

    role = begin_request.role;
    flags = begin_request.flags;
    request_id = header.request_id;

    transceiver.ConsumePadding(header);

    return;
}

/**
 * This is the core of the request, a set of key/value parameters that defines the request we need to respond to.
 */
void Request::ProcessParams(model::Header const& header)
{
    size_t read_counter = 0;
    while (read_counter < header.content_length) {
        int32_t name_len = GetVariableSizeLength(read_counter);
        int32_t value_len = GetVariableSizeLength(read_counter);

        char data_name[name_len];
        transceiver.Stream().ReadBytes(data_name, name_len);
        read_counter += name_len;

        if (value_len > 0) {
            char data_value[value_len];
            transceiver.Stream().ReadBytes(data_value, value_len);
            read_counter += value_len;
            params.insert_or_assign(std::string(data_name, name_len), std::string(data_value, value_len));
        } else {
            params.insert_or_assign(std::string(data_name, name_len), std::string());
        }
    }

    ValidateRecordLength(header, read_counter);
    transceiver.ConsumePadding(header);
}

/**
 * This is post data sent from the server.
 *
 * TODO:
 *  - Consider a max file-size limit here
 *  - Where do we put the file?
 */
void Request::ProcessIn(model::Header const& header)
{
    in_data += transceiver.ReadStream(header);
}

/**
 * Ensure we are receiving the expected record length for fixed-length records.
 */
Request& Request::ValidateRecordLength(model::Header const& header, size_t expected)
{
    if (header.content_length != expected) {
        throw PayloadException("FastCGI record length mismatch. Expected: " + std::to_string(expected) +
                               ", actual: " + std::to_string(header.content_length));
    }

    return *this;
}

uint32_t Request::GetVariableSizeLength(size_t& read_counter)
{
    // 32-bit buffer, but we might only use one byte
    unsigned char buffer[4];
    transceiver.Stream().ReadBytes(buffer, 1);

    // 8-bit size
    if (buffer[0] >> 7 == 0) {
        ++read_counter;
        return buffer[0];
    }

    // 32-bit size
    transceiver.Stream().ReadBytes(buffer + 1, 3);
    read_counter += 4;
    return bes_endian_u32(*buffer, true);
}

model::Role Request::Role() const
{
    return role;
}

uint8_t Request::Flags() const
{
    return flags;
}

std::unordered_map<std::string, std::string> const& Request::Params() const
{
    return params;
}

std::string const& Request::Param(std::string const& key) const
{
    auto const& it = params.find(key);
    if (it != params.end()) {
        return it->second;
    } else {
        throw IndexErrorException("Parameter '" + key + "' does not exist");
    }
}

bool Request::HasParam(std::string const& key) const
{
    return params.find(key) != params.end();
}

uint16_t Request::RequestId() const
{
    return request_id;
}
