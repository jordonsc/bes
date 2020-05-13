#ifndef BES_WEB_WEB_SERVER_H
#define BES_WEB_WEB_SERVER_H

#include <memory>

#include "bes/fastcgi.h"
#include "router.h"
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

   protected:
    std::unique_ptr<bes::fastcgi::Service> svc;
    std::shared_ptr<std::vector<std::shared_ptr<Router>>> routers;
};

template <class T, class... Args>
inline void WebServer::EmplaceRouter(Args &&... args)
{
    routers->push_back(std::make_shared<T>(std::forward<Args>(args)...));
}

}  // namespace bes::web

#endif
