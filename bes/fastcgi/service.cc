#include "service.h"

using namespace bes::fastcgi;

Service::~Service()
{
    Shutdown();
}

Service& Service::Run(bes::net::Address const& listen_addr, size_t threads, size_t socket_queue_len)
{
    if (svr_running.load()) {
        throw FastCgiException("FastCGI service already running");
    }

    BES_LOG(INFO) << "Starting FastCGI listener..";
    main_socket.Bind(listen_addr);
    worker_pool = std::make_unique<bes::ThreadPool>(threads);
    svr_running.store(true);

    main_socket.ListenAsync(
        [this](bes::net::socket::Stream&& socket) {
            if (!svr_running.load()) {
                BES_LOG(ERROR) << "Server not running, cannot start listener";
                return;
            }

            worker_pool->Enqueue([this, socket{std::move(socket)}]() mutable {
                try {
                    Transceiver tns(socket);
                    Request req(tns, container);

                    // Process the request
                    if (!req.Run()) {
                        // Was aborted
                        tns.SendEndRequest(model::ProtoStatus::REQUEST_COMPLETE, EXIT_FAILURE);
                        return;
                    }

                    // Process the response
                    if (role_factories[static_cast<uint16_t>(req.Role()) - 1] == nullptr) {
                        // Respond with unsupported role
                        tns.SendEndRequest(model::ProtoStatus::UNKNOWN_ROLE);
                    } else {
                        // Build the role and generate a response
                        auto role = role_factories[static_cast<uint16_t>(req.Role()) - 1](req, tns);
                        auto app_code = role->Run();
                        role->Flush(true);
                        tns.SendEndRequest(model::ProtoStatus::REQUEST_COMPLETE, app_code);
                    }
                } catch (std::exception& e) {
                    BES_LOG(ERROR) << "FCGI error processing: " << e.what();
                }
            });
        },
        socket_queue_len);

    return *this;
}

Service& Service::Run(std::string const& addr, uint16_t port, size_t threads, size_t socket_queue_len)
{
    return Run(bes::net::Address(addr, port), threads, socket_queue_len);
}

Service& Service::Shutdown()
{
    if (svr_running.load()) {
        main_socket.Stop();
        main_socket.Close();
        worker_pool.reset(nullptr);
        svr_running.store(false);
    }

    return *this;
}
