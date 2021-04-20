#pragma once

#include <bes/app.h>

#include <memory>
#include <queue>

#include "exception.h"
#include "rpc_controller.tcc"

namespace bes::rpc {

template <class RpcService>
class RpcApplication : public bes::app::Application
{
   public:
    void run() override;
    void shutdown() override;
    virtual ~RpcApplication();

   protected:
    using Application::Application;

    std::unique_ptr<bes::rpc::RpcController<RpcService>> rpc_ctrl;

    /**
     * Bind the server to a given IP address and run it.
     *
     * Override this to change the default binding or config behaviour.
     */
    virtual void bindServer();

    /**
     * Register an RPC handler for listening.
     *
     * The handler must be an RPC of the given service, and the svc_key is the key used in the application's
     * configuration file, whereby it will use the values defined in the configuration to scale threads and performance.
     */
    template <class HandlerClass>
    void registerRpcHandler(std::string const& rpc_id);

   private:
    std::queue<std::function<void()>> handler_stack;
};

template <class RpcService>
void RpcApplication<RpcService>::run()
{
    if (rpc_ctrl != nullptr) {
        throw bes::app::KernelPanicException("Panic! Attempted to rebuild service controller!");
    }

    rpc_ctrl = std::make_unique<bes::rpc::RpcController<RpcService>>(this);
    bindServer();

    // Request the implementation now execute all RPC handlers
    while (!handler_stack.empty()) {
        handler_stack.front()();
        handler_stack.pop();
    }
}

template <class RpcService>
void RpcApplication<RpcService>::bindServer()
{
    if (rpc_ctrl == nullptr) {
        throw bes::app::KernelPanicException("Panic! RPC Controller not created, cannot bind.");
    }

    // The default port we can reconfigure with the below macro, but the default bind address will always be controlled
    // by the service controller. This will be 0.0.0.0 as there is really no need to ever change this.
    rpc_ctrl.get()->run(
        bes::net::Address(kernel().getConfig().getOr<std::string>("0.0.0.0", "server", "bind"), "",
                          kernel().getConfig().getOr<unsigned>(BES_SERVER_DEFAULT_PORT, "server", "listen")),
        handler_stack.size());
}

template <class RpcService>
void RpcApplication<RpcService>::shutdown()
{
    if (rpc_ctrl != nullptr) {
        rpc_ctrl->shutdown();
    }
}

template <class RpcService>
template <class HandlerClass>
void RpcApplication<RpcService>::registerRpcHandler(std::string const& rpc_id)
{
    handler_stack.push([this, rpc_id] {
        if (rpc_ctrl == nullptr) {
            throw std::runtime_error("Cannot register RPC handler before initialisation completed and server running");
        }

        auto max_threads = kernel().getConfig().getOr<unsigned>(10, "rpc", rpc_id, "threads");
        if (max_threads < 1) {
            BES_LOG(ALERT) << "Invalid thread count for RPC handler '" << rpc_id << "', NOT accepting connections!";
            return;
        }

        rpc_ctrl.get()->template handleRpc<HandlerClass>(max_threads);
        BES_LOG(INFO) << "RPC handler '" << rpc_id << "' running with " << max_threads << " threads";
    });
}

template <class RpcService>
RpcApplication<RpcService>::~RpcApplication()
{
    RpcApplication<RpcService>::shutdown();
}

}  // namespace bes::rpc

