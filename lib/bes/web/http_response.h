#pragma once

#include <sstream>
#include <unordered_map>

#include "cookie.h"
#include "http.h"

namespace bes::web {

class HttpResponse
{
   public:
    HttpResponse() = default;
    HttpResponse(HttpResponse&&) = default;
    HttpResponse& operator=(HttpResponse&&) = default;

    static HttpResponse ok(std::string const& content_type = Http::ContentType::HTML);

    /**
     * Set an HTTP header.
     */
    void header(std::string const& key, std::string const& value);

    /**
     * Get all headers.
     */
    std::unordered_map<std::string, std::string> const& headers() const;

    /**
     * Sets the HTTP status code, and optionally the content-type.
     */
    void status(Http::Status status_code);
    void status(Http::Status status_code, std::string const& content_type);

    /**
     * Set a cookie to send to the client.
     */
    void setCookie(Cookie cookie);

    /**
     * Get a map of all cookies
     */
    std::unordered_map<std::string, Cookie> const& cookies() const;

    /**
     * Write content to the internal content buffer. Returns the length of content written.
     */
    size_t write(std::string const& data);

    /**
     * Get the content.
     */
    std::string content() const;

   protected:
    std::unordered_map<std::string, std::string> http_headers;
    std::unordered_map<std::string, Cookie> http_cookies;
    std::stringstream resp_content;
};

}  // namespace bes::web
