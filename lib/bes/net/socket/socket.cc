#include "socket.h"

using namespace bes::net::socket;

bool Socket::bind(::bes::net::Address const& addr, bool reuse)
{
    if (is_bound.load()) {
        throw bes::net::SocketBindException("Cannot bind an already bound socket");
    }

    if (!is_open.load()) {
        open();
    }

    std::unique_lock<std::mutex> lock(bind_mutex);

    if (reuse) {
        ::setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &int_true, sizeof(int_true));
    } else {
        ::setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &int_false, sizeof(int_false));
    }

    ::memset(&sock_addr, 0, sizeof(sock_addr));
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = ::htons(addr.port());
    sock_addr.sin_addr.s_addr = ::inet_addr(addr.ip4Addr().c_str());

    if (::bind(sock, (struct ::sockaddr*)&sock_addr, sizeof(sock_addr)) == -1) {
        throw bes::net::SocketBindException("Unable to bind: " + addr.ip4AddrFull());
    }

    return true;
}

void Socket::close()
{
    if (!is_open.load()) {
        return;
    }

    std::unique_lock<std::mutex> lock(bind_mutex);

    ::close(sock);

    sock = 0;
    is_bound.store(false);
}

void Socket::open()
{
    if (is_open.load()) {
        throw bes::net::SocketBindException("Cannot open an already open socket");
    }

    std::unique_lock<std::mutex> lock(bind_mutex);

    auto [fam, s_type, proto] = getSocketOptions();
    sock = ::socket(fam, s_type, proto);

    if (sock == -1) {
        throw ::bes::net::SocketException("Unable to create socket");
    }
}

Socket::~Socket()
{
    close();
}
