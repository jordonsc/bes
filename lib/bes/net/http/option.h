#pragma once

#include <curlpp/Easy.hpp>
#include <utility>

namespace bes::net::http {

class Response;

/**
 * Options for HTTP requests.
 */
class Option
{
   private:
    virtual void hydrate(curlpp::Easy& request) const = 0;
    friend class Response;
};

}  // namespace bes::net::http
