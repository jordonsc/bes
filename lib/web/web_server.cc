#include "web_server.h"

using namespace bes::web;

WebServer::WebServer()
{
    routers = std::make_shared<std::vector<std::shared_ptr<Router>>>();
}

void WebServer::Run(bes::net::Address const& listen_addr, bool allow_dbg_rendering)
{
    BES_LOG(INFO) << "Starting web server on " << listen_addr.AddrFull() << "..";
    // Start the FastCGI server
    svc = std::make_unique<bes::fastcgi::Service>();
    svc->container.Add("routers", routers);
    svc->container.Emplace<bool>("debug_mode", allow_dbg_rendering);
    svc->SetRole<WebResponder>(bes::fastcgi::model::Role::RESPONDER);
    svc->Run(listen_addr);
}

void WebServer::Shutdown()
{
    if (svc != nullptr) {
        svc->Shutdown();
        svc.reset(nullptr);
    }
}

void WebServer::AllocateRouter(Router* router)
{
    routers->push_back(std::shared_ptr<Router>(router));
}

void WebServer::AddRouter(std::shared_ptr<Router> const& router)
{
    routers->push_back(router);
}
