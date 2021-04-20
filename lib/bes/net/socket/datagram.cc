#include "datagram.h"

using namespace bes::net::socket;

socket_opt_t Datagram::getSocketOptions()
{
    return {AF_INET, SOCK_DGRAM, IPPROTO_UDP};
}

bes::net::Message Datagram::receive()
{
    socklen_t len = sizeof(sock_addr);
    ssize_t payload_len = recvfrom(sock, rec_buffer, sizeof(rec_buffer), 0, (struct ::sockaddr*)&sock_addr, &len);
    if (payload_len == -1) {
        throw bes::net::SocketException("Receive error");
    }

    return bes::net::Message(std::string(rec_buffer, payload_len),
                             bes::net::Address(::inet_ntoa(sock_addr.sin_addr), "", ::ntohs(sock_addr.sin_port)));
}

size_t Datagram::dispatch(bes::net::Message const& payload, bool broadcast)
{
    if (is_bound.load()) {
        throw SocketException("Cannot dispatch on a bound socket");
    }

    ::setsockopt(sock, SOL_SOCKET, SO_BROADCAST, broadcast ? &int_true : &int_false, sizeof(int));
    ::memset(&sock_addr, 0, sizeof(sock_addr));
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = ::htons(payload.address.port());
    sock_addr.sin_addr.s_addr = ::inet_addr(payload.address.ip4Addr().c_str());

    // Fire the packet off
    char const* send_buffer = payload.payload.c_str();
    ssize_t sent =
        sendto(sock, send_buffer, payload.payload.length(), 0, (struct sockaddr*)&sock_addr, sizeof(sock_addr));

    if (sent < 0) {
        throw SocketException("Error dispatching datagram to " + payload.address.addrFull());
    }

    return sent;
}
