#ifndef BES_WEB_HTTP_REQUEST_H
#define BES_WEB_HTTP_REQUEST_H

#include <unordered_map>

#include "http.h"
#include "bes/fastcgi.h"

namespace bes::web {

class HttpRequest
{
   public:
    explicit HttpRequest(fastcgi::Request const& base);

    std::string const& Uri() const;
    std::string const& QueryString() const;
    Http::Method const& Method() const;
    bool HasQueryParam(std::string const& key) const;
    std::string QueryParam(std::string const& key) const;

   protected:
    bes::fastcgi::Request const& base_request;
    std::unordered_map<std::string, std::string> query_params;
    Http::Method method;

   private:
    void ParseQueryString();
    static char HexToChar(char c);
};

}  // namespace bes::web

#endif
