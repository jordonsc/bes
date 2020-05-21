#ifndef BES_WEB_WEB_RESPONDER_H
#define BES_WEB_WEB_RESPONDER_H

#include <bes/fastcgi.h>
#include <bes/log.h>

#include <chrono>

#include "exception.h"
#include "http.h"
#include "http_request.h"
#include "model.h"
#include "router.h"
#include "session_interface.h"

namespace bes::web {

class WebResponder : public bes::fastcgi::Response
{
    using bes::fastcgi::Response::Response;

   public:
    int Run() override;

   protected:
    void RenderResponse(HttpResponse const& resp, HttpRequest const& req);
    void RenderCookie(Cookie const&);
    void RenderError(HttpRequest const& req, Http::Status code, std::string const& debug_msg);
    void RenderEmergencyErrorResponse(std::string const& debug_msg);
    bool DebugMode();
};

}  // namespace bes::web

#endif
