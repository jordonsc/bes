#include "http_response.h"

using namespace bes::web;

void HttpResponse::status(Http::Status status_code)
{
    return header(Http::Header::STATUS, std::to_string(static_cast<int>(status_code)));
}

void HttpResponse::status(Http::Status status_code, std::string const& content_type)
{
    header(Http::Header::STATUS, std::to_string(static_cast<int>(status_code)));
    header(Http::Header::CONTENT_TYPE, content_type);
}

void HttpResponse::header(std::string const& key, std::string const& value)
{
    http_headers.insert_or_assign(key, value);
}

std::unordered_map<std::string, std::string> const& HttpResponse::headers() const
{
    return http_headers;
}

size_t HttpResponse::write(std::string const& data)
{
    resp_content << data;

    return data.length();
}

std::string HttpResponse::content() const
{
    return resp_content.str();
}

void HttpResponse::setCookie(Cookie cookie)
{
    http_cookies.insert_or_assign(cookie.getName(), std::move(cookie));
}

std::unordered_map<std::string, Cookie> const& HttpResponse::cookies() const
{
    return http_cookies;
}

HttpResponse HttpResponse::ok(std::string const& content_type)
{
    HttpResponse ok;
    ok.status(Http::Status::OK, content_type);

    return ok;
}
