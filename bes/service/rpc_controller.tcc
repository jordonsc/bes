#ifndef BES_SERVICE_SERVICE_CONTROLLER_TCC
#define BES_SERVICE_SERVICE_CONTROLLER_TCC

#include <grpcpp/grpcpp.h>

#include <cassert>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "bes/core.h"
#include "bes/log.h"
#include "bes/net.h"
#include "pool_tracker.h"

namespace bes::service {

using threadsize_t = bes::threadsize_t;

/**
 * This is a managed controller for RPC applications.
 *
 * It enables an application to listen for RPC requests in a thread-pool and easily respond to them via the
 * ServiceHandler class template.
 */
template <class ServiceClass>
class RpcController
{
   public:
    ~RpcController();

    /**
     * Build and run the server.
     *
     * We need to know how many completion queues to build, so `num_queues` is the number of RPC handlers we'll start
     * after calling Run(). This number _MUST_ match the number of calls to HandleRpc() exactly.
     */
    void Run(bes::net::Address const& listen_addr, size_t num_queues);

    // When C++20 is available, we can add a parameter pack for HandlerT constructor args (new lambda feature)
    template <class HandlerT>
    void HandleRpc(threadsize_t max_threads);

    // Stop listening for connections and shutdown the server
    void Shutdown();

   private:
    bool svr_alive = false;
    std::vector<std::unique_ptr<grpc::ServerCompletionQueue>> cq_vec;
    std::vector<std::unique_ptr<grpc::ServerCompletionQueue>>::const_iterator cq_index;
    ServiceClass service;
    std::unique_ptr<grpc::Server> server;
    std::vector<std::thread> rpc_threads;
    std::mutex safety_mutex;
};

template <class ServiceClass>
void RpcController<ServiceClass>::Run(bes::net::Address const& listen_addr, size_t num_queues)
{
    std::lock_guard<std::mutex> lock(safety_mutex);
    svr_alive = true;

    grpc::ServerBuilder builder;

    // Listen on the given address without any authentication mechanism
    // TODO: look into credentials
    builder.AddListeningPort(listen_addr.Ip4AddrFull(), grpc::InsecureServerCredentials());

    // `service` is the GRPC service object we're going to respond to on this server
    builder.RegisterService(&service);

    // We need a completion queue for every RPC we're going to respond to
    for (size_t i = 0; i < num_queues; ++i) {
        cq_vec.push_back(builder.AddCompletionQueue());
    }
    cq_index = cq_vec.cbegin();

    // Assemble and fire-up the server
    server = builder.BuildAndStart();

    BES_LOG(NOTICE) << "Server listening on " << listen_addr.Ip4AddrFull();
}

template <class ServiceClass>
template <class HandlerT>
void RpcController<ServiceClass>::HandleRpc(threadsize_t const max_threads)
{
    std::lock_guard<std::mutex> lock(safety_mutex);

    if (!svr_alive) {
        BES_LOG(ALERT) << "Requesting RPC handling while server NOT running. RPC request NOT listening!";
        return;
    }

    // Min number of handler classes that should be ready and requesting RPC calls
    threadsize_t min_handlers = 2;

    if (max_threads < min_handlers) {
        BES_LOG(ALERT) << "Thread count of " << max_threads << " is below minimum of " << min_handlers
                       << ", aborting RPC request!";
        return;
    }

    // Run a loop in a thread,
    rpc_threads.push_back(std::thread([&, this, min_handlers, max_threads] {
        // Current number of processors we've running for this RPC
        bes::service::PoolTracker tracker;
        bes::ThreadPool thread_pool(max_threads);
        std::unique_ptr<grpc::ServerCompletionQueue> const& cq = *cq_index;
        ++cq_index;

        // Spawn a new CallData instance to serve new clients.
        void* tag;  // uniquely identifies a request.
        bool ok = true;

        while (true) {
            if (tracker.Count() < 0) {
                // If this ever drops below zero, something real funny is happening, log for debugging
                BES_LOG(CRITICAL) << "Thread tracker has dropped below zero";
            }

            /**
             * The CQ will set OK to false during a shutdown, in which we don't want to spawn new handlers.
             */
            while (ok && (tracker.Capacity() < min_handlers)) {
                if (tracker.Count() >= max_threads) {
                    // At capacity
                    // TODO: log this somewhere (statsd or something, don't use the logger)
                    break;
                }

                (new HandlerT(&service, cq.get(), &tracker))->Proceed();
            }

            /**
             * This is a blocking call to get activity off the completion queue. If it returns negative, then the queue
             * has been shutdown AND is exhausted, making it now safe for us to exit.
             */
            if (!cq->Next(&tag, &ok)) {
                // Server shutting down
                BES_LOG(TRACE) << "Exiting RPC loop for shutdown";
                break;
            }

            /**
             * ok == true indicates that we should be processing this message. If we are draining the queue for
             * shutdown, then `ok` will be false, and we'll skip further processing.
             */
            if (ok) {
                // Have a request to process (could be either process or clean-up)
                thread_pool.Enqueue(
                    [](void* ref) {
                        static_cast<HandlerT*>(ref)->Proceed();
                    },
                    tag);
            }
        }
    }));
}

template <class ServiceClass>
inline RpcController<ServiceClass>::~RpcController()
{
    Shutdown();
}

template <class ServiceClass>
inline void RpcController<ServiceClass>::Shutdown()
{
    std::lock_guard<std::mutex> lock(safety_mutex);

    if (svr_alive) {
        svr_alive = false;

        // Shutdown the server before cleaning up the CQ
        BES_LOG(INFO) << "Closing RPC connections..";
        // TODO: consider this being a config option
        server->Shutdown(std::chrono::system_clock::now() + std::chrono::seconds(3));

        // Always shutdown the completion queue after the server
        for (auto& cq : cq_vec) {
            cq->Shutdown();
        }

        for (auto& t : rpc_threads) {
            if (t.joinable()) {
                t.join();
            }
        }
    }
}

}  // namespace bes::service

#endif
