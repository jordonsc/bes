#pragma once

#include <bes/log.h>
#include <bes/net.h>

#include <cstring>

#include "memory.tcc"
#include "model.h"

namespace bes::fastcgi {

class Transceiver
{
   public:
    explicit Transceiver(bes::net::socket::Stream& socket);

    template <class T>
    T readModel(bool to_host = true);

    template <class T>
    void writeModel(T& model, bool to_net = true);

    std::string readStream(model::Header const& header);

    void writeStream(model::RecordType, std::string const& data, int16_t request_id);

    /**
     * Send an EndRequest to the server.
     */
    void sendEndRequest(model::ProtoStatus exit_code, int32_t app_code = EXIT_SUCCESS);

    /**
     * Read past the bytes for a record we don't care to do anything with.
     */
    void skipRecord(model::Header const& header)
    {
        char buffer[header.content_length + header.padding_length];
        socket.readBytes(&buffer, header.content_length + header.padding_length);
    }

    /**
     * Consume leftover padding in a record.
     */
    void consumePadding(model::Header const& header);

    /**
     * Get the underlying Stream object.
     */
    bes::net::socket::Stream& stream();

   protected:
    bes::net::socket::Stream& socket;
};

template <class T>
inline T Transceiver::readModel(bool to_host)
{
    T model;
    socket.readBytes(&model, sizeof(T));

    if (to_host) {
        endian<T>(model, true);
    }

    return model;
}

template <class T>
inline void Transceiver::writeModel(T& model, bool to_net)
{
    if (to_net) {
        endian<T>(model, true);
    }

    socket.writeBytes(&model, sizeof(T));
}

}  // namespace bes::fastcgi

