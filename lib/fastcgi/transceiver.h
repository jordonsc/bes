#ifndef BES_FASTCGI_TRANSCEIVER_H
#define BES_FASTCGI_TRANSCEIVER_H

#include <cstring>

#include "lib/log/log.h"
#include "lib/net/net.h"
#include "memory.tcc"
#include "model.h"

namespace bes::fastcgi {

class Transceiver
{
   public:
    Transceiver(bes::net::socket::Stream& socket);

    template <class T>
    T ReadModel(bool to_host = true);

    template <class T>
    void WriteModel(T& model, bool to_net = true);

    std::string ReadStream(model::Header const& header);

    void WriteStream(model::RecordType, std::string const& data, int16_t request_id);

    /**
     * Send an EndRequest to the server.
     */
    void SendEndRequest(model::ProtoStatus exit_code, int32_t app_code = EXIT_SUCCESS);

    /**
     * Read past the bytes for a record we don't care to do anything with.
     */
    void SkipRecord(model::Header const& header)
    {
        char buffer[header.content_length + header.padding_length];
        socket.ReadBytes(&buffer, header.content_length + header.padding_length);
        return;
    }

    /**
     * Consume leftover padding in a record.
     */
    void ConsumePadding(model::Header const& header);

    /**
     * Get the underlying Stream object.
     */
    bes::net::socket::Stream& Stream();

   protected:
    bes::net::socket::Stream& socket;
};

template <class T>
inline T Transceiver::ReadModel(bool to_host)
{
    T model;
    socket.ReadBytes(&model, sizeof(T));

    if (to_host) {
        Endian<T>(model, true);
    }

    return model;
}

template <class T>
inline void Transceiver::WriteModel(T& model, bool to_net)
{
    if (to_net) {
        Endian<T>(model, true);
    }

    socket.WriteBytes(&model, sizeof(T));
}


}  // namespace bes::fastcgi

#endif
