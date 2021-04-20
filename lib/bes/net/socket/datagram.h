#pragma once

#include "../address.h"
#include "../message.h"
#include "socket.h"

namespace bes::net::socket {

/**
 * UDP socket wrapper for broadcasting and receiving datagrams.
 *
 * Unix support only. All functions are synchronous and not thread-safe.
 * Cannot copy, but safe to move.
 */
class Datagram : public Socket
{
   public:
    Datagram(Datagram const&) = delete;
    Datagram& operator=(Datagram const&) = delete;

    /**
     * Blocking datagram receiver.
     *
     * Will throw an exception if the socket is not yet bound.
     */
    bes::net::Message receive();

    /**
     * Send a datagram.
     *
     * If `broadcast` is set to true, then the UDP socket is configured for broadcasting. It must be true if you intend
     * on dispatching to multiple clients.
     *
     * Will throw an exception if the socket has already been bound for listening purposes.
     */
    size_t dispatch(bes::net::Message const& payload, bool broadcast = false);

   protected:
    socket_opt_t getSocketOptions() override;
    char rec_buffer[255];
};

}  // namespace bes::net::socket
