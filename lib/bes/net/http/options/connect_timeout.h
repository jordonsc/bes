#pragma once

#include <curlpp/Options.hpp>

#include "../option.h"

namespace bes::net::http::option {

/**
 * Connect-phase timeout.
 *
 * Value in seconds. Defaults to 300.
 */
class ConnectTimeout : public Option
{
   public:
    explicit ConnectTimeout(unsigned long v) : value(v) {}

   private:
    void hydrate(curlpp::Easy& request) const override
    {
        request.setOpt<curlpp::options::ConnectTimeout>(value);
    }

    unsigned long value;
};

}  // namespace bes::net::http::option
