#pragma once

#include <bes/log.h>
#include <bes/net.h>
#include <bes/web.h>
#include <yaml-cpp/yaml.h>

#include <cpp_redis/cpp_redis>
#include <vector>

namespace bes::web {

class RedisSessionMgr : public SessionInterface
{
   public:
    explicit RedisSessionMgr(bes::net::Address svr, uint32_t timeout_ms = 250);
    RedisSessionMgr(std::vector<bes::net::Address> const& sentinels, std::string sentinel_svc,
                    uint32_t timeout_ms = 250);
    static RedisSessionMgr* fromConfig(bes::Config const& config);

    void setSessionTtl(uint64_t ttl) override;

    [[nodiscard]] Session createSession(std::string const& ns) override;
    [[nodiscard]] Session getSession(std::string const& id) override;
    void persistSession(Session const& session) override;

   protected:
    void connect();
    RedisSessionMgr& logConnectStatus(std::string const& host, std::size_t port, cpp_redis::connect_state status);

    cpp_redis::client client;
    bes::net::Address server;
    std::string sentinel_svc_name;
    uint32_t connect_timeout;
    uint64_t session_ttl = 0;
};

}  // namespace bes::web
