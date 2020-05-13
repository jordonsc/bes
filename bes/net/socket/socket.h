#ifndef BES_NET_SOCKET_SOCKET_H
#define BES_NET_SOCKET_SOCKET_H

#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>

#include <atomic>
#include <mutex>

#include "../address.h"
#include "../exception.h"

namespace bes::net::socket {

using socket_opt_t = std::tuple<int, int, int>;

class Socket
{
   public:
    bool Bind(::bes::net::Address const& addr, bool reuse = true);
    void Open();
    void Close();
    virtual ~Socket();

    [[nodiscard]] inline int UnderlyingSocket() const {
        return sock;
    }

   protected:
    virtual socket_opt_t GetSocketOptions() = 0;

    constexpr static int const int_false = 0;
    constexpr static int const int_true = 1;

    sockaddr_in sock_addr;
    int sock;

    std::atomic<bool> open{false};
    std::atomic<bool> bound{false};

   private:
    std::mutex bind_mutex;
};

}  // namespace bes::net::socket

#endif