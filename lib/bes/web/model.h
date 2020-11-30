#ifndef BES_WEB_MODEL_H
#define BES_WEB_MODEL_H

namespace bes::web {

auto constexpr DEBUG_KEY = "debug_mode";
auto constexpr SVC_SESSION_MGR = "session_mgr";
auto constexpr SVC_ROUTER = "routers";
auto constexpr SESSION_TTL_KEY = "session_ttl";
auto constexpr SESSION_SECURE_KEY = "session_secure";

auto constexpr SESSION_COOKIE_KEY = "session_cookie_name";
auto constexpr SESSION_PREFIX_KEY = "session_prefix";

auto constexpr SESSION_DEFAULT_COOKIE = "bsn";
auto constexpr SESSION_DEFAULT_PREFIX = "S";

}  // namespace bes::web

#endif
