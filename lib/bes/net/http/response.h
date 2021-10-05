#pragma once

#include <curlpp/Infos.hpp>
#include <ostream>

#include "types.h"

namespace bes::net::http {

class Response
{
   public:
    Response(Response&&) = default;
    Response(Response const&) = delete;
    Response& operator=(Response const&) = delete;
    Response& operator=(Response&&) = default;

    [[nodiscard]] int httpCode() const
    {
        return curlpp::infos::ResponseCode::get(*request);
    }

    [[nodiscard]] std::string body() const
    {
        return os->str();
    }

   protected:
    explicit Response(std::string url)
        : req_url(std::move(url)),
          request(std::make_shared<curlpp::Easy>()),
          os(std::make_shared<std::ostringstream>())
    {
    }

    void setOptions(std::vector<opt_ptr> const& options)
    {
        for (auto& option : options) {
            option->hydrate(*request);
        }
    }

    void execute()
    {
        request->setOpt(curlpp::options::WriteStream(&*os));
        request->setOpt<curlpp::options::Url>(req_url);

        try {
            request->perform();
        } catch (std::exception const& e) {
            throw std::runtime_error(std::string("shit: ") + e.what());
        }
    }

    std::string req_url;
    std::shared_ptr<curlpp::Easy> request;
    std::shared_ptr<std::ostringstream> os;

    friend class Request;
};

}  // namespace bes::net::http
