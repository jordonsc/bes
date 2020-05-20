#ifndef BES_WEB_REDIS_REDIS_SESSION_MGR_H
#define BES_WEB_REDIS_REDIS_SESSION_MGR_H

#include <cpp_redis/cpp_redis>
#include <vector>

#include "bes/log.h"
#include "bes/net.h"
#include "bes/web.h"

namespace bes::web {

class RedisSessionMgr : public SessionInterface
{
   public:
    explicit RedisSessionMgr(bes::net::Address svr, uint32_t timeout_ms = 250);
    RedisSessionMgr(std::vector<bes::net::Address> const& sentinels, std::string sentinel_svc,
                    uint32_t timeout_ms = 250);
    static RedisSessionMgr* FromConfig(bes::Config const&);

    void SetSessionTtl(uint64_t ttl) override;

    [[nodiscard]] Session CreateSession() override;
    [[nodiscard]] Session GetSession(std::string const& id) override;
    void PersistSession(Session const& session) override;

   protected:
    void Connect();
    RedisSessionMgr& LogConnectStatus(std::string const& host, std::size_t port, cpp_redis::connect_state status);

    cpp_redis::client client;
    bes::net::Address server;
    std::string sentinel_svc_name;
    uint32_t connect_timeout;
    uint64_t session_ttl = 0;
};

}  // namespace bes::web

#endif
