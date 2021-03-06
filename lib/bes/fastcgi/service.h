#pragma once

#include <bes/core.h>
#include <bes/log.h>

#include <atomic>
#include <cstring>
#include <memory>
#include <mutex>

#include "bes/net.h"
#include "exception.h"
#include "model.h"
#include "request.h"
#include "response.h"

namespace bes::fastcgi {

class Service
{
   public:
    virtual ~Service();

    Service& run(bes::net::Address const& listen_addr, size_t threads = 10, size_t socket_queue_len = 5);
    Service& run(std::string const& addr, uint16_t port, size_t threads = 10, size_t socket_queue_len = 5);
    Service& shutdown();

    template <class T>
    Service& setRole(model::Role role);

    bes::Container container;

   protected:
    std::function<std::shared_ptr<Response>(const Request&, Transceiver&)> role_factories[3];

    bes::net::socket::Stream main_socket;
    std::atomic<bool> svr_running{false};

    std::unique_ptr<bes::ThreadPool> worker_pool;

   private:
};

template <class T>
inline Service& Service::setRole(model::Role role)
{
    role_factories[static_cast<uint16_t>(role) - 1] = [](Request const& request, Transceiver& tns) {
        return std::make_shared<T>(request, tns);
    };

    return *this;
}

}  // namespace bes::fastcgi

