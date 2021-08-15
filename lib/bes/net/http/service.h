#pragma once

#include "options.h"
#include "request.h"

namespace bes::net::http {

/**
 * A service to generate templated requests using a common option base.
 *
 * Ideally this would be a singleton, constructed for a DI container, etc.
 */
class HttpService
{
    void setOption();

    HttpRequest newRequest();
};

}  // namespace bes::net::http