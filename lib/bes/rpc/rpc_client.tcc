#ifndef BES_RPC_SERVICECLIENT_H
#define BES_RPC_SERVICECLIENT_H

#include <bes/app.h>
#include <grpcpp/grpcpp.h>

#include <memory>

namespace bes::rpc {

template <class RpcService>
class RpcClient
{
   public:
    explicit RpcClient(std::shared_ptr<bes::app::discovery::DiscoveryInterface> const& discovery_iface = nullptr)
        : discovery_iface(discovery_iface ? discovery_iface
                                          : bes::app::discovery::DiscoveryInterface::GetDiscoveryInterface())
    {}

   protected:
    /**
     * Get the stub for making the RPC call.
     *
     * This will request the stub built if it hasn't already been (stub lazy-loader).
     */
    virtual typename RpcService::Stub GetStub()
    {
        auto adr = discovery_iface.get()->GetServiceAddr(GetServiceKey()).AddrFull();
        auto channel = grpc::CreateChannel(adr, grpc::InsecureChannelCredentials());
        return typename RpcService::Stub(channel);
    }

    /**
     * Overload this to return the service name you're attempting to reach. This key will be passed to the service
     * discovery service to convert to a hostname.
     */
    virtual std::string const& GetServiceKey() = 0;

    // Service discovery interface; really should only be used for building the stub
    std::shared_ptr<bes::app::discovery::DiscoveryInterface> discovery_iface;
};

}  // namespace bes::rpc

#endif
