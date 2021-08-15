#pragma once

#include <curlpp/Easy.hpp>
#include <curlpp/Infos.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/cURLpp.hpp>

namespace bes::net::http {

class HttpRequest
{
    /**
     * Execute HTTP request asynchronously.
     *
     * Returns a future.
     *
     * @param url
     */
    void execute(std::string const& url)
    {
        curlpp::Easy request;
        std::ostringstream os;
        curlpp::options::WriteStream ws(&os);

        request.setOpt(ws);
        request.setOpt<curlpp::options::Url>(url);
        request.setOpt<curlpp::options::Timeout>(5);

        request.perform();

        int const http_code = curlpp::infos::ResponseCode::get(request);
        std::string const body = os.str();
    }
};

}  // namespace bes::net::http