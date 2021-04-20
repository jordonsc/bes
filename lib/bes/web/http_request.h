#pragma once

#include <bes/fastcgi.h>

#include <cctype>
#include <unordered_map>

#include "cookie.h"
#include "exception.h"
#include "http.h"
#include "model.h"
#include "session_interface.h"

namespace bes::web {

class HttpRequest
{
   public:
    explicit HttpRequest(fastcgi::Request const& base, std::string const& session_prefix = "S");
    ~HttpRequest();

    /**
     * URI _not_ including the query-string
     */
    [[nodiscard]] std::string const& uri() const;

    /**
     * Query-string segment of the URI (everything after the ?)
     */
    [[nodiscard]] std::string const& queryString() const;

    /**
     * HTTP Method of request (GET, POST, etc)
     */
    [[nodiscard]] Http::Method const& method() const;

    /**
     * Check if we have a query-string parameter (aka "GET" param)
     */
    [[nodiscard]] bool hasQueryParam(std::string const& key) const;

    /**
     * Get the value of a query-string parameter, throwing a std::out_of_range exception if it doesn't exist.
     */
    [[nodiscard]] std::string const& queryParam(std::string const& key) const;

    /**
     * Check for a cookie :)
     */
    [[nodiscard]] bool hasCookie(std::string const& key) const;

    /**
     * Get the value of a cookie, throwing a std::out_of_range exception if it doesn't exist.
     */
    [[nodiscard]] std::string const& getCookie(std::string const& key) const;

    /**
     * Check for a FastCGI parameter
     */
    [[nodiscard]] bool hasParam(std::string const& key) const;

    /**
     * Get the value of a FastCGI parameter, throwing a std::out_of_range exception if it doesn't exist.
     */
    [[nodiscard]] std::string const& getParam(std::string const& key) const;

    /**
     * Check if we have an existing session.
     */
    [[nodiscard]] bool hasSession() const;

    /**
     * Get the session, create one if it didn't exist.
     */
    Session& getSession() const;

   protected:
    bes::fastcgi::Request const& base_request;
    Http::Method http_method;
    std::unordered_map<std::string, std::string> query_params;
    std::unordered_map<std::string, std::string> cookies;
    mutable Session session;

   private:
    void parseQueryString();
    void parseCookies();
    void bootstrapSession(std::string const& prefix = "S");
    static char hexToChar(char c);
};

}  // namespace bes::web
