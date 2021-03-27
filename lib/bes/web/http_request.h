#ifndef BES_WEB_HTTP_REQUEST_H
#define BES_WEB_HTTP_REQUEST_H

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
    explicit HttpRequest(fastcgi::Request const& base);
    ~HttpRequest();

    /**
     * URI _not_ including the query-string
     */
    [[nodiscard]] std::string const& Uri() const;

    /**
     * Query-string segment of the URI (everything after the ?)
     */
    [[nodiscard]] std::string const& QueryString() const;

    /**
     * HTTP Method of request (GET, POST, etc)
     */
    [[nodiscard]] Http::Method const& Method() const;

    /**
     * Check if we have a query-string parameter (aka "GET" param)
     */
    [[nodiscard]] bool HasQueryParam(std::string const& key) const;

    /**
     * Get the value of a query-string parameter, throwing a std::out_of_range exception if it doesn't exist.
     */
    [[nodiscard]] std::string const& QueryParam(std::string const& key) const;

    /**
     * Check for a cookie :)
     */
    [[nodiscard]] bool HasCookie(std::string const& key) const;

    /**
     * Get the value of a cookie, throwing a std::out_of_range exception if it doesn't exist.
     */
    [[nodiscard]] std::string const& GetCookie(std::string const& key) const;

    /**
     * Check for a FastCGI parameter
     */
    [[nodiscard]] bool HasParam(std::string const& key) const;

    /**
     * Get the value of a FastCGI parameter, throwing a std::out_of_range exception if it doesn't exist.
     */
    [[nodiscard]] std::string const& GetParam(std::string const& key) const;

    /**
     * Check if we have an existing session.
     */
    [[nodiscard]] bool HasSession() const;

    /**
     * Get the session, create one if it didn't exist.
     */
    Session& GetSession() const;

   protected:
    bes::fastcgi::Request const& base_request;
    Http::Method method;
    std::unordered_map<std::string, std::string> query_params;
    std::unordered_map<std::string, std::string> cookies;
    mutable Session session;

   private:
    void ParseQueryString();
    void ParseCookies();
    void BootstrapSession();
    static char HexToChar(char c);
};

}  // namespace bes::web

#endif
