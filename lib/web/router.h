#ifndef BES_WEB_ROUTER_H
#define BES_WEB_ROUTER_H

#include "exception.h"
#include "http.h"
#include "http_request.h"
#include "http_response.h"

namespace bes::web {

using Controller = std::function<HttpResponse(HttpRequest const&, ActionArgs const&)>;

class Router
{
   public:
    virtual HttpResponse YieldResponse(HttpRequest const& request) const = 0;
    virtual HttpResponse YieldErrorResponse(HttpRequest const& request, Http::Status status_code,
                                            std::string const& debug_msg) const = 0;
};


}  // namespace bes::web

#endif
