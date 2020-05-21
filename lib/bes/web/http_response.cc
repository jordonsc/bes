#include "http_response.h"

using namespace bes::web;

void HttpResponse::Status(Http::Status status_code)
{
    return Header(Http::Header::STATUS, std::to_string(static_cast<int>(status_code)));
}

void HttpResponse::Status(Http::Status status_code, std::string const& content_type)
{
    Header(Http::Header::STATUS, std::to_string(static_cast<int>(status_code)));
    Header(Http::Header::CONTENT_TYPE, content_type);
}

void HttpResponse::Header(std::string const& key, std::string const& value)
{
    headers.insert_or_assign(key, value);
}

std::unordered_map<std::string, std::string> const& HttpResponse::Headers() const
{
    return headers;
}

size_t HttpResponse::Write(std::string const& data)
{
    content << data;

    return data.length();
}

std::string HttpResponse::Content() const
{
    return content.str();
}

void HttpResponse::SetCookie(Cookie cookie)
{
    cookies.insert_or_assign(cookie.Name(), std::move(cookie));
}

std::unordered_map<std::string, Cookie> const& HttpResponse::Cookies() const
{
    return cookies;
}

HttpResponse HttpResponse::OK(std::string const& content_type)
{
    HttpResponse ok;
    ok.Status(Http::Status::OK, content_type);

    return ok;
}
