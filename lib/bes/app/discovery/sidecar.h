#pragma once

#include <bes/core.h>
#include <bes/net.h>

#include <unordered_map>

#include "discovery_interface.h"

namespace bes::app::discovery {

/**
 * The Sidecar service-discovery model is suitable for most cluster environments.
 *
 * This interface will assume you've got a proxy on the node which will do a DNS resolution from a service alias. Common
 * examples of this would be the Envoy proxy or the Kubernetes 'kube-proxy`.
 *
 * This interface also allows for config file overriding, allowing us to get an address from the config instead of proxy
 * server. It also allows us to configure how the hostname for the service will look, such as adding a TLD or a
 * namespace.
 *
 * By default this simply returns the service name almost verbatim depending on your configuration, assuming that your
 * node will convert that to an IP address.
 */
class Sidecar : public DiscoveryInterface
{
   public:
    explicit Sidecar(bes::Config const& config);

    bes::net::Address getServiceAddr(std::string const& service) override;
    bes::net::Address getServiceAddr(std::string const& service, std::string const& ns) override;

    bool ready() override;

   protected:
    bes::Config const& config;
    uint16_t port;
    std::string tld;
    std::string ns;
    std::unordered_map<std::string, bes::net::Address> cache;
};

}  // namespace bes::app::discovery
