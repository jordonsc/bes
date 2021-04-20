#include "web_server.h"

using namespace bes::web;

WebServer::WebServer()
{
    routers = std::make_shared<std::vector<std::shared_ptr<Router>>>();
}

void WebServer::run(bes::net::Address const& listen_addr, bool allow_dbg_rendering)
{
    BES_LOG(INFO) << "Binding web server to " << listen_addr.addrFull() << "..";
    // Start the FastCGI server
    svc = std::make_unique<bes::fastcgi::Service>();
    svc->container.add(SVC_ROUTER, routers);
    svc->container.add(SVC_SESSION_MGR, session_mgr);
    svc->container.emplace<bool>(DEBUG_KEY, allow_dbg_rendering);
    svc->container.emplace<uint64_t>(SESSION_TTL_KEY, session_ttl);
    svc->container.emplace<bool>(SESSION_SECURE_KEY, session_secure);
    svc->container.emplace<std::string>(SESSION_PREFIX_KEY, SESSION_DEFAULT_PREFIX);
    svc->container.emplace<std::string>(SESSION_COOKIE_KEY, SESSION_DEFAULT_COOKIE);
    svc->setRole<WebResponder>(bes::fastcgi::model::Role::RESPONDER);
    svc->run(listen_addr);
}

void WebServer::shutdown()
{
    if (svc != nullptr) {
        svc->shutdown();
        svc.reset(nullptr);
    }
}

void WebServer::allocateRouter(Router* router)
{
    routers->push_back(std::shared_ptr<Router>(router));
}

void WebServer::addRouter(std::shared_ptr<Router> const& router)
{
    routers->push_back(router);
}

void WebServer::allocateSessionInterface(SessionInterface* si)
{
    session_mgr = std::shared_ptr<SessionInterface>(si);
    session_mgr->setSessionTtl(session_ttl);
}

void WebServer::setSessionInterface(std::shared_ptr<SessionInterface> const& si)
{
    session_mgr = si;
    session_mgr->setSessionTtl(session_ttl);
}

void WebServer::setSessionTtl(uint64_t ttl)
{
    session_ttl = ttl;

    if (svc != nullptr) {
        if (svc->container.exists(SESSION_TTL_KEY)) {
            svc->container.remove(SESSION_TTL_KEY);
        }
        svc->container.emplace<uint64_t>(SESSION_TTL_KEY, ttl);
    }

    if (session_mgr != nullptr) {
        session_mgr->setSessionTtl(ttl);
    }
}

void WebServer::setSessionSecure(bool secure)
{
    session_secure = secure;

    if (svc != nullptr) {
        if (svc->container.exists(SESSION_SECURE_KEY)) {
            svc->container.remove(SESSION_SECURE_KEY);
        }
        svc->container.emplace<bool>(SESSION_SECURE_KEY, secure);
    }
}

void WebServer::setSessionPrefix(std::string const& prefix)
{
    if (svc != nullptr) {
        if (svc->container.exists(SESSION_PREFIX_KEY)) {
            svc->container.remove(SESSION_PREFIX_KEY);
        }
        svc->container.emplace<std::string>(SESSION_PREFIX_KEY, prefix);
    }
}

void WebServer::setSessionCookieName(std::string const& name)
{
    if (svc != nullptr) {
        if (svc->container.exists(SESSION_COOKIE_KEY)) {
            svc->container.remove(SESSION_COOKIE_KEY);
        }
        svc->container.emplace<std::string>(SESSION_COOKIE_KEY, name);
    }
}
