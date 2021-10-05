#pragma once

#include <curlpp/Options.hpp>
#include <string>

#include "../option.h"

namespace bes::net::http::option {

/**
 * Proxy server.
 *
 * Example: `ProxyOption("http://user:password@someserver:8080");`
 */
class Proxy : public Option
{
   public:
    explicit Proxy(std::string proxy) : proxy(std::move(proxy)) {}
    // Proxy(std::string proxy, unsigned long port) : proxy(std::move(proxy)), port(port) {}

   private:
    void hydrate(curlpp::Easy& request) const override
    {
        request.setOpt<curlpp::options::Proxy>(proxy);
        // request.setOpt<curlpp::options::ProxyPort>(port);
    }

    std::string proxy;
};

}  // namespace bes::net::http::option
