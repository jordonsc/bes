#pragma once

#include <bes/fastcgi.h>

#include <memory>

#include "model.h"
#include "router.h"
#include "session_interface.h"
#include "web_responder.h"

namespace bes::web {

class WebServer
{
   public:
    WebServer();
    void run(bes::net::Address const &listen_addr, bool allow_dbg_rendering = false);
    void shutdown();

    template <class T, class... Args>
    void emplaceRouter(Args &&... args);
    void allocateRouter(Router *router);
    void addRouter(std::shared_ptr<Router> const &router);

    template <class T, class... Args>
    void emplaceSessionInterface(Args &&... args);
    void allocateSessionInterface(SessionInterface *si);
    void setSessionInterface(std::shared_ptr<SessionInterface> const &si);

    void setSessionTtl(uint64_t ttl);
    void setSessionSecure(bool);

    void setSessionPrefix(std::string const &prefix);
    void setSessionCookieName(std::string const &name);

   protected:
    std::unique_ptr<bes::fastcgi::Service> svc;
    std::shared_ptr<std::vector<std::shared_ptr<Router>>> routers;
    std::shared_ptr<SessionInterface> session_mgr;
    uint64_t session_ttl = 0;
    bool session_secure = false;
};

template <class T, class... Args>
inline void WebServer::emplaceRouter(Args &&... args)
{
    routers->push_back(std::make_shared<T>(std::forward<Args>(args)...));
}

template <class T, class... Args>
inline void WebServer::emplaceSessionInterface(Args &&... args)
{
    session_mgr = std::make_shared<T>(std::forward<Args>(args)...);
    session_mgr->setSessionTtl(session_ttl);
}

}  // namespace bes::web

