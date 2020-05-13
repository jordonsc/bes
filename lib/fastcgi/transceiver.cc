#include "transceiver.h"

using namespace bes::fastcgi;

Transceiver::Transceiver(bes::net::socket::Stream& socket) : socket(socket) {}

std::string Transceiver::ReadStream(model::Header const& header)
{
    if (header.content_length) {
        char buffer[header.content_length];
        socket.ReadBytes(buffer, header.content_length);
        ConsumePadding(header);
        return std::string(buffer, header.content_length);
    } else {
        ConsumePadding(header);
        return std::string();
    }
}

void Transceiver::WriteStream(model::RecordType rt, std::string const& data, int16_t request_id)
{
    size_t written = 0;
    size_t payload_size = data.length();
    char const* buffer = data.c_str();

    do {
        size_t seg_size = data.length() - written;
        if (seg_size > 64000) {
            seg_size = 64000;
        }

        // Create stream header
        model::Header header;
        header.content_length = seg_size;
        header.padding_length = seg_size % model::chunk_size ? model::chunk_size - (seg_size % model::chunk_size) : 0;
        header.request_id = request_id;
        header.version = model::fcgi_version;
        header.type = rt;
        header.reserved = '\0';

        // Endian transform & send
        WriteModel(header);

        // Send stream data
        if (seg_size) {
            socket.WriteBytes(buffer + written, seg_size);
        }

        // Send padding data (NB: we can only still use padding_length because it's 8-bit)
        if (header.padding_length) {
            char pad[header.padding_length];
            std::memset(pad, '\0', header.padding_length);
            socket.WriteBytes(pad, header.padding_length);
        }

        written += seg_size;

    } while (written != payload_size);
}

/**
 * Write an EndRequest.
 */
void Transceiver::SendEndRequest(model::ProtoStatus exit_code, int32_t app_code)
{
    model::Header header;
    header.request_id = 1;
    header.type = model::RecordType::END_REQUEST;
    header.version = model::fcgi_version;
    header.content_length = sizeof(model::EndRequest);
    header.padding_length =
        header.content_length % model::chunk_size ? model::chunk_size - (header.content_length % model::chunk_size) : 0;
    header.reserved = '\0';
    WriteModel(header);

    model::EndRequest end_req;
    end_req.protocol_status = exit_code;
    end_req.app_status = app_code;
    std::memset(end_req.reserved, '\0', 3);
    WriteModel(end_req);
}

void Transceiver::ConsumePadding(model::Header const& header)
{
    if (!header.padding_length) {
        return;
    }

    unsigned char buffer[header.padding_length];
    socket.ReadBytes(buffer, header.padding_length);
}

bes::net::socket::Stream& Transceiver::Stream()
{
    return socket;
}
