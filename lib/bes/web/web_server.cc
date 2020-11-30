#include "web_server.h"

using namespace bes::web;

WebServer::WebServer()
{
    routers = std::make_shared<std::vector<std::shared_ptr<Router>>>();
}

void WebServer::Run(bes::net::Address const& listen_addr, bool allow_dbg_rendering)
{
    BES_LOG(INFO) << "Binding web server to " << listen_addr.AddrFull() << "..";
    // Start the FastCGI server
    svc = std::make_unique<bes::fastcgi::Service>();
    svc->container.Add(SVC_ROUTER, routers);
    svc->container.Add(SVC_SESSION_MGR, session_mgr);
    svc->container.Emplace<bool>(DEBUG_KEY, allow_dbg_rendering);
    svc->container.Emplace<uint64_t>(SESSION_TTL_KEY, session_ttl);
    svc->container.Emplace<bool>(SESSION_SECURE_KEY, session_secure);
    svc->container.Emplace<std::string>(SESSION_PREFIX_KEY, SESSION_DEFAULT_PREFIX);
    svc->container.Emplace<std::string>(SESSION_COOKIE_KEY, SESSION_DEFAULT_COOKIE);
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

void WebServer::AllocateSessionInterface(SessionInterface* si)
{
    session_mgr = std::shared_ptr<SessionInterface>(si);
    session_mgr->SetSessionTtl(session_ttl);
}

void WebServer::SetSessionInterface(std::shared_ptr<SessionInterface> const& si)
{
    session_mgr = si;
    session_mgr->SetSessionTtl(session_ttl);
}

void WebServer::SetSessionTtl(uint64_t ttl)
{
    session_ttl = ttl;

    if (svc != nullptr) {
        if (svc->container.Exists(SESSION_TTL_KEY)) {
            svc->container.Remove(SESSION_TTL_KEY);
        }
        svc->container.Emplace<uint64_t>(SESSION_TTL_KEY, ttl);
    }

    if (session_mgr != nullptr) {
        session_mgr->SetSessionTtl(ttl);
    }
}

void WebServer::SetSessionSecure(bool secure)
{
    session_secure = secure;

    if (svc != nullptr) {
        if (svc->container.Exists(SESSION_SECURE_KEY)) {
            svc->container.Remove(SESSION_SECURE_KEY);
        }
        svc->container.Emplace<bool>(SESSION_SECURE_KEY, secure);
    }
}

void WebServer::SetSessionPrefix(std::string const& prefix){
    if (svc != nullptr) {
        if (svc->container.Exists(SESSION_PREFIX_KEY)) {
            svc->container.Remove(SESSION_PREFIX_KEY);
        }
        svc->container.Emplace<std::string>(SESSION_PREFIX_KEY, prefix);
    }
}

void WebServer::SetSessionCookieName(std::string const& name){
    if (svc != nullptr) {
        if (svc->container.Exists(SESSION_COOKIE_KEY)) {
            svc->container.Remove(SESSION_COOKIE_KEY);
        }
        svc->container.Emplace<std::string>(SESSION_COOKIE_KEY, name);
    }
}
