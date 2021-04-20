#pragma once

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
    int run() override;

   protected:
    void renderResponse(HttpResponse const& resp, HttpRequest const& req);
    void renderCookie(Cookie const& cookie);
    void renderError(HttpRequest const& req, Http::Status code, std::string const& debug_msg);
    void renderEmergencyErrorResponse(std::string const& debug_msg);
    bool debugMode();
};

}  // namespace bes::web
