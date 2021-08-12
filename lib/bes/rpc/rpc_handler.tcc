#pragma once

#include <bes/log.h>
#include <grpcpp/grpcpp.h>

#include <iostream>
#include <stdexcept>
#include <thread>

#include "pool_tracker.h"

using grpc::Server;
using grpc::ServerAsyncResponseWriter;
using grpc::ServerBuilder;
using grpc::ServerCompletionQueue;
using grpc::ServerContext;
using grpc::Status;

namespace bes::rpc {

/**
 * This is the foundation for listening to individual RPC calls.
 *
 * For every RPC call you'll need an implementation of this class, and send it to the ServiceController for it to
 * manage in a thread-pool and process the small state-engine implemented in this template.
 */
template <class SyncSvcT, class RequestT, class ResponseT>
class RpcHandler
{
   public:
    // Take in the "service" instance (in this case representing an asynchronous server) and the completion queue
    // used for asynchronous communication with the gRPC runtime.
    RpcHandler(SyncSvcT* svc, ServerCompletionQueue* cq, PoolTracker* trk, void* app);

    virtual ~RpcHandler();

    void proceed();

   protected:
    /**
     * Should contain a single call to the appropriate server->RequestXxxx(...) function.
     *
     * eg:
     * service->RequestHelloWorld(&context, &request, &responder, completion_queue, completion_queue, this);
     */
    virtual void requestRpc() = 0;

    /**
     * The core logic for the RPC will go in here.
     *
     * Will be executed in a thread, keep atomic.
     */
    virtual grpc::Status process() = 0;

    RequestT request;    // What we get from the client
    ResponseT response;  // What we send back to the client

    // The means of communication with the gRPC runtime for an asynchronous server
    SyncSvcT* service;

    // The producer-consumer queue where for asynchronous server notifications
    ServerCompletionQueue* completion_queue;

    PoolTracker* tracker;

    // The means to get back to the client.
    ServerAsyncResponseWriter<ResponseT> responder;

    // Context for the rpc, allowing to tweak aspects of it such as the use of compression, authentication, as well as
    // to send metadata back to the client.
    ServerContext context;

    // Reference to the application
    void* application;

   private:
    // State of the request
    enum class CallStatus
    {
        CREATE,
        PROCESS,
        FINISH
    } status = CallStatus::CREATE;
};

template <class SyncSvcT, class RequestT, class ResponseT>
inline RpcHandler<SyncSvcT, RequestT, ResponseT>::RpcHandler(SyncSvcT* svc, ServerCompletionQueue* cq, PoolTracker* trk,
                                                             void* application)
    : service(svc), completion_queue(cq), tracker(trk), responder(&context), application(application)
{
    // New instance is offering capacity
    tracker->instanceSpawning(this);
}

template <class SyncSvcT, class RequestT, class ResponseT>
inline void RpcHandler<SyncSvcT, RequestT, ResponseT>::proceed()
{
    if (status == CallStatus::CREATE) {
        // Request a call from the gRPC engine
        requestRpc();
        status = CallStatus::PROCESS;

    } else if (status == CallStatus::PROCESS) {
        // We've got an incoming RPC call, inform the tracker that we're no longer offering capacity
        tracker->instanceWorking(this);
        grpc::Status rpc_status = process();

        // Let the gRPC runtime know we've finished and pass it our reply (formed in Process())
        responder.Finish(response, rpc_status, this);
        status = CallStatus::FINISH;

    } else if (status == CallStatus::FINISH) {
        // Once in the FINISH state, deallocate ourselves
        delete this;

    } else {
        // Throw an exception
        throw std::runtime_error("Unknown service handler state");
    }
}

template <class SyncSvcT, class RequestT, class ResponseT>
inline RpcHandler<SyncSvcT, RequestT, ResponseT>::~RpcHandler()
{
    // Register this instance as no longer consuming resources or offering capacity
    tracker->instanceTerminating(this);
}

}  // namespace bes::rpc
