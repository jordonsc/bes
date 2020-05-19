#ifndef BES_WEB_REDIS_REDIS_SESSION_H
#define BES_WEB_REDIS_REDIS_SESSION_H

#include <cpp_redis/cpp_redis>

#include "bes/log.h"
#include "bes/net.h"
#include "bes/web.h"

namespace bes::web {

class RedisSession : public SessionInterface
{
   public:
    RedisSession(bes::net::Address svr);

    Session CreateSession() override;
    Session GetSession(std::string const& id) override;
    void PersistSession(Session const& session, size_t ttl) override;

   protected:
    void Connect();

    cpp_redis::client client;
    bes::net::Address server;
};

}  // namespace bes::web

#endif
