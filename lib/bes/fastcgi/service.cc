#include "service.h"

using namespace bes::fastcgi;

Service::~Service()
{
    shutdown();
}

Service& Service::run(bes::net::Address const& listen_addr, size_t threads, size_t socket_queue_len)
{
    if (svr_running.load()) {
        throw FastCgiException("FastCGI service already running");
    }

    BES_LOG(INFO) << "Starting FastCGI server..";
    main_socket.bind(listen_addr);
    worker_pool = std::make_unique<bes::ThreadPool>(threads);
    svr_running.store(true);

    main_socket.listenAsync(
        [this](bes::net::socket::Stream&& socket) {
            if (!svr_running.load()) {
                BES_LOG(ERROR) << "Server not running, cannot start listener";
                return;
            }

            worker_pool->enqueue([this, socket{std::move(socket)}]() mutable {
                try {
                    Transceiver tns(socket);
                    Request req(tns, container);

                    // Process the request
                    if (!req.run()) {
                        // Was aborted
                        tns.sendEndRequest(model::ProtoStatus::REQUEST_COMPLETE, EXIT_FAILURE);
                        return;
                    }

                    // Process the response
                    if (role_factories[static_cast<uint16_t>(req.getRole()) - 1] == nullptr) {
                        // Respond with unsupported role
                        tns.sendEndRequest(model::ProtoStatus::UNKNOWN_ROLE);
                    } else {
                        // Build the role and generate a response
                        auto role = role_factories[static_cast<uint16_t>(req.getRole()) - 1](req, tns);
                        auto app_code = role->run();
                        role->flush(true);
                        tns.sendEndRequest(model::ProtoStatus::REQUEST_COMPLETE, app_code);
                    }
                } catch (std::exception& e) {
                    BES_LOG(ERROR) << "FCGI error processing: " << e.what();
                }
            });
        },
        socket_queue_len);

    return *this;
}

Service& Service::run(std::string const& addr, uint16_t port, size_t threads, size_t socket_queue_len)
{
    return run(bes::net::Address(addr, port), threads, socket_queue_len);
}

Service& Service::shutdown()
{
    if (svr_running.load()) {
        main_socket.stop();
        main_socket.close();
        worker_pool.reset(nullptr);
        svr_running.store(false);
    }

    return *this;
}
