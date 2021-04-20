#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <atomic>
#include <cstring>
#include <mutex>

#include "../address.h"
#include "../exception.h"

namespace bes::net::socket {

using socket_opt_t = std::tuple<int, int, int>;

class Socket
{
   public:
    virtual ~Socket();

    bool bind(::bes::net::Address const& addr, bool reuse = true);
    void open();
    void close();

    [[nodiscard]] inline int underlyingSocket() const
    {
        return sock;
    }

   protected:
    virtual socket_opt_t getSocketOptions() = 0;

    constexpr static int const int_false = 0;
    constexpr static int const int_true = 1;

    sockaddr_in sock_addr;
    int sock;

    std::atomic<bool> is_open{false};
    std::atomic<bool> is_bound{false};

   private:
    std::mutex bind_mutex;
};

}  // namespace bes::net::socket
