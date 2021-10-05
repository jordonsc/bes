#pragma once

#include <curlpp/Options.hpp>

#include "../option.h"

namespace bes::net::http::option {

/**
 * Request timeout.
 *
 * Value in seconds. Defaults to 300.
 */
class Timeout : public Option
{
   public:
    explicit Timeout(unsigned long v) : value(v) {}

   private:
    void hydrate(curlpp::Easy& request) const override
    {
        request.setOpt<curlpp::options::Timeout>(value);
    }

    unsigned long value;
};

}  // namespace bes::net::http::option
