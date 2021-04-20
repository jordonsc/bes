#pragma once

#include "exception.h"
#include "http.h"
#include "http_request.h"
#include "http_response.h"

namespace bes::web {

using Controller = std::function<HttpResponse(HttpRequest const&, ActionArgs const&)>;

class Router
{
   public:
    [[nodiscard]] virtual HttpResponse yieldResponse(HttpRequest const& request) const = 0;
    [[nodiscard]] virtual HttpResponse yieldErrorResponse(HttpRequest const& request, Http::Status status_code,
                                                          std::string const& debug_msg) const = 0;
};

}  // namespace bes::web
