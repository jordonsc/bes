#include "sidecar.h"

using namespace bes::service::discovery;

Sidecar::Sidecar(bes::Config const& config) : config(config)
{
    port = config.GetOr<uint16_t>(BES_SERVER_DEFAULT_PORT, "discovery", "config", "port");
    tld = config.GetOr<std::string>("", "discovery", "config", "tld");
    ns = config.GetOr<std::string>("", "discovery", "config", "ns");
}

bes::net::Address Sidecar::GetServiceAddr(std::string const& service, std::string const& namespc)
{
    // Service ID is "service_name.namespace" unless there is no default or passed namespace
    std::string svc_id = namespc.length() ? service + "." + namespc : (ns.length() ? service + "." + ns : service);

    try {
        return cache.at(svc_id);
    } catch (std::out_of_range&) {
    }

    std::string svc_tld;
    try {
        svc_tld = config.GetOrNull<std::string>(tld, "discovery", "service", svc_id, "tld");
    } catch (bes::NullException&) {
        // The TLD has been explicitly removed from this service
        svc_tld = "";
    }

    bes::net::Address val(config.GetOr<std::string>(svc_tld.length() ? svc_id + "." + svc_tld : svc_id, "discovery",
                                                    "service", svc_id, "host"),
                          config.GetOr<uint16_t>(port, "discovery", "service", svc_id, "port"));

    cache.insert_or_assign(service, val);

    return val;
}

bool Sidecar::Ready()
{
    return true;
}
