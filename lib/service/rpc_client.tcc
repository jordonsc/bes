#ifndef BES_SERVICE_SERVICECLIENT_H
#define BES_SERVICE_SERVICECLIENT_H

#include <grpcpp/grpcpp.h>

#include <memory>

namespace bes::service {

template <class RpcService>
class RpcClient
{
   public:
    RpcClient(std::shared_ptr<bes::service::discovery::DiscoveryInterface> discovery_iface = nullptr)
        : discovery_iface(discovery_iface ? discovery_iface
                                          : bes::service::discovery::DiscoveryInterface::GetDiscoveryInterface())
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
#if BES_ENV == 0
        BES_LOG(TRACE) << "Svc '" << GetServiceKey() << "' resolved to '" << adr << "'";
#endif
        auto channel = grpc::CreateChannel(adr, grpc::InsecureChannelCredentials());
        return typename RpcService::Stub(channel);
    }

    /**
     * Overload this to return the service name you're attempting to reach. This key will be passed to the service
     * discovery service to convert to a hostname.
     */
    virtual std::string const& GetServiceKey() = 0;

    // Service discovery interface; really should only be used for building the stub
    std::shared_ptr<bes::service::discovery::DiscoveryInterface> discovery_iface;
};

}  // namespace bes::service

#endif
