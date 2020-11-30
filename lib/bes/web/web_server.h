#ifndef BES_WEB_WEB_SERVER_H
#define BES_WEB_WEB_SERVER_H

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
    void Run(bes::net::Address const &listen_addr, bool allow_dbg_rendering = false);
    void Shutdown();

    template <class T, class... Args>
    void EmplaceRouter(Args &&...);
    void AllocateRouter(Router *);
    void AddRouter(std::shared_ptr<Router> const &);

    template <class T, class... Args>
    void EmplaceSessionInterface(Args &&...);
    void AllocateSessionInterface(SessionInterface *);
    void SetSessionInterface(std::shared_ptr<SessionInterface> const &);

    void SetSessionTtl(uint64_t ttl);
    void SetSessionSecure(bool);

    void SetSessionPrefix(std::string const &prefix);
    void SetSessionCookieName(std::string const &name);

   protected:
    std::unique_ptr<bes::fastcgi::Service> svc;
    std::shared_ptr<std::vector<std::shared_ptr<Router>>> routers;
    std::shared_ptr<SessionInterface> session_mgr;
    uint64_t session_ttl = 0;
    bool session_secure = false;
};

template <class T, class... Args>
inline void WebServer::EmplaceRouter(Args &&... args)
{
    routers->push_back(std::make_shared<T>(std::forward<Args>(args)...));
}

template <class T, class... Args>
inline void WebServer::EmplaceSessionInterface(Args &&... args)
{
    session_mgr = std::make_shared<T>(std::forward<Args>(args)...);
    session_mgr->SetSessionTtl(session_ttl);
}

}  // namespace bes::web

#endif
