#ifndef BES_SERVICE_DISCOVERY_INTERFACE_H
#define BES_SERVICE_DISCOVERY_INTERFACE_H

#include <bes/log.h>
#include <bes/net.h>
#include <grpcpp/grpcpp.h>

#include <memory>
#include <shared_mutex>

#include "../exception.h"

namespace bes::app::discovery {

class DiscoveryInterface
{
   public:
    /**
     * Retrieve the address for a given service by key.
     */
    virtual bes::net::Address GetServiceAddr(std::string const& service) = 0;
    virtual bes::net::Address GetServiceAddr(std::string const& service, std::string const& ns) = 0;

    /**
     * Return true if the interface is online and ready for discovery.
     *
     * The kernel will check if the interface is ready, if not, it will delay and give it some time for repetitively
     * checking again. During the time, the interface should be communicating with a control plane, etc.
     */
    virtual bool Ready() = 0;

    /**
     * Request all processes exit and we block until complete
     */
    virtual inline void Shutdown(){};

    /**
     * Creates a default discovery interface all services can use.
     */
    template <class T, class... Args>
    static void SetDiscoveryInterface(Args&&... args)
    {
        std::lock_guard<std::shared_mutex> lock(DiscoveryInterface::discovery_m);
        DiscoveryInterface::discovery_iface = std::make_unique<T>(std::forward<Args>(args)...);
    }

    static void SetDiscoveryInterface(std::shared_ptr<DiscoveryInterface> iface)
    {
        std::lock_guard<std::shared_mutex> lock(DiscoveryInterface::discovery_m);
        DiscoveryInterface::discovery_iface = iface;
    }

    static std::shared_ptr<DiscoveryInterface> GetDiscoveryInterface()
    {
        std::shared_lock<std::shared_mutex> lock(DiscoveryInterface::discovery_m);
        if (DiscoveryInterface::discovery_iface == nullptr) {
            throw NoDiscoveryInterfaceException("No service discovery interface created");
        }

        return DiscoveryInterface::discovery_iface;
    }

    static bool HasDiscoveryInterface()
    {
        std::shared_lock<std::shared_mutex> lock(DiscoveryInterface::discovery_m);
        return DiscoveryInterface::discovery_iface != nullptr;
    }

   private:
    static std::shared_ptr<DiscoveryInterface> discovery_iface;
    static std::shared_mutex discovery_m;
};

}  // namespace bes::service::discovery

#endif
