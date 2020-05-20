#ifndef BES_WEB_MODEL_H
#define BES_WEB_MODEL_H

#ifndef SESSION_ID
#define SESSION_ID "SESSION_ID"
#endif

namespace bes::web {

auto constexpr SESSION_KEY = SESSION_ID;
auto constexpr DEBUG_KEY = "debug_mode";
auto constexpr SVC_SESSION_MGR = "session_mgr";
auto constexpr SVC_ROUTER = "routers";

}  // namespace bes::web

#endif
