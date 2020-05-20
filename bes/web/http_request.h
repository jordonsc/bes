#ifndef BES_WEB_HTTP_REQUEST_H
#define BES_WEB_HTTP_REQUEST_H

#include <unordered_map>

#include "bes/fastcgi.h"
#include "http.h"

namespace bes::web {

class HttpRequest
{
   public:
    explicit HttpRequest(fastcgi::Request const& base);

    std::string const& Uri() const;
    std::string const& QueryString() const;
    Http::Method const& Method() const;

    bool HasQueryParam(std::string const& key) const;
    std::string const& QueryParam(std::string const& key) const;

    bool HasCookie(std::string const& key) const;
    std::string const& Cookie(std::string const& key) const;

   protected:
    bes::fastcgi::Request const& base_request;
    Http::Method method;
    std::unordered_map<std::string, std::string> query_params;
    std::unordered_map<std::string, std::string> cookies;

   private:
    void ParseQueryString();
    void ParseCookies();
    static char HexToChar(char c);
};

}  // namespace bes::web

#endif
