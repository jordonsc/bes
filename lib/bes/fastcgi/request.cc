#include "request.h"

using namespace bes::fastcgi;

Request::Request(Transceiver& tns, bes::Container const& cnt) : container(cnt), transceiver(tns) {}

bool Request::run()
{
    try {
        // Process all input records
        while (processRecord()) {
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
bool Request::processRecord()
{
    auto header = transceiver.readModel<model::Header>();

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
            BES_LOG(DEBUG) << "FCGI: processing BEGIN";
            processBeginRequest(header);
            return true;

        case model::RecordType::ABORT_REQUEST:
            BES_LOG(DEBUG) << "FCGI: processing ABORT";
            transceiver.skipRecord(header);
            throw AbortException("Server aborted request");

        case model::RecordType::PARAMS:
            BES_LOG(DEBUG) << "FCGI: processing PARAMS";
            processParams(header);
            return true;

        case model::RecordType::IN:
            BES_LOG(DEBUG) << "FCGI: processing IN";
            processIn(header);
            return false;

        default:
            BES_LOG(WARNING) << "FCGI: server skipping record-type: " << int(static_cast<uint8_t>(header.type));
            transceiver.skipRecord(header);
            return true;
    }
}

/**
 * The BeginRequest segment marks the intended role and sets some flags.
 */
void Request::processBeginRequest(model::Header const& header)
{
    validateRecordLength(header, sizeof(model::BeginRequest));

    auto begin_request = transceiver.readModel<model::BeginRequest>();

    role = begin_request.role;
    flags = begin_request.flags;
    request_id = header.request_id;

    transceiver.consumePadding(header);
}

/**
 * This is the core of the request, a set of key/value parameters that defines the request we need to respond to.
 */
void Request::processParams(model::Header const& header)
{
    size_t read_counter = 0;
    while (read_counter < header.content_length) {
        auto name_len = getVariableSizeLength(read_counter);
        auto value_len = getVariableSizeLength(read_counter);

        char data_name[name_len];
        transceiver.stream().readBytes(data_name, name_len);
        read_counter += name_len;

        if (value_len > 0) {
            char data_value[value_len];
            transceiver.stream().readBytes(data_value, value_len);
            read_counter += value_len;
            params.insert_or_assign(std::string(data_name, name_len), std::string(data_value, value_len));
        } else {
            params.insert_or_assign(std::string(data_name, name_len), std::string());
        }
    }

    validateRecordLength(header, read_counter);
    transceiver.consumePadding(header);
}

/**
 * This is post data sent from the server.
 *
 * TODO:
 *  - Consider a max file-size limit here
 *  - Where do we put the file?
 */
void Request::processIn(model::Header const& header)
{
    in_data += transceiver.readStream(header);
}

/**
 * Ensure we are receiving the expected record length for fixed-length records.
 */
Request& Request::validateRecordLength(model::Header const& header, size_t found)
{
    if (header.content_length != found) {
        throw PayloadException("FastCGI record length mismatch. Read: " + std::to_string(found) +
                               ", expected: " + std::to_string(header.content_length));
    }

    return *this;
}

/**
 * Reads either 1 or 4 bytes, depending on the first bit received.
 *
 * The very first bit defines if we're using 8-bit or 32-bit encoding.
 *  - If it's zero, we're using an 8-bit value (really a 7-bit number, so a max-length of 127)
 *  - If it's non-zero, we're using a 32-bit value (really 31-bit, max length 2.1e9)
 *
 * See also: http://www.mit.edu/~yandros/doc/specs/fcgi-spec.html#S3.4
 */
uint32_t Request::getVariableSizeLength(size_t& read_counter)
{
    // 32-bit buffer, but we might only use one byte
    unsigned char buffer[4];
    transceiver.stream().readBytes(buffer, 1);

    // 8-bit size
    if (buffer[0] >> 7 == 0) {
        ++read_counter;
        return buffer[0];
    }

    // 32-bit size
    transceiver.stream().readBytes(buffer + 1, 3);
    read_counter += 4;

    uint32_t value;
    buffer[0] ^= 128;   // zero-out the first bit which is the byte-count indicator
    std::memcpy(&value, buffer, 4);

    return bes_endian_u32(value, true);
}

model::Role Request::getRole() const
{
    return role;
}

uint8_t Request::getFlags() const
{
    return flags;
}

std::unordered_map<std::string, std::string> const& Request::getParams() const
{
    return params;
}

std::string const& Request::getParam(std::string const& key) const
{
    auto const& it = params.find(key);
    if (it != params.end()) {
        return it->second;
    } else {
        throw IndexErrorException("Parameter '" + key + "' does not exist");
    }
}

bool Request::hasParam(std::string const& key) const
{
    return params.find(key) != params.end();
}

uint16_t Request::getRequestId() const
{
    return request_id;
}
