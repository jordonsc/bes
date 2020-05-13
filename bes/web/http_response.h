#ifndef BES_WEB_HTTP_RESPONSE_H
#define BES_WEB_HTTP_RESPONSE_H

#include <sstream>
#include <unordered_map>

#include "http.h"

namespace bes::web {

class HttpResponse
{
   public:
    /**
     * Set an HTTP header.
     */
    void Header(std::string const& key, std::string const& value);

    /**
     * Get all headers.
     */
    std::unordered_map<std::string, std::string> const& Headers() const;

    /**
     * Sets the HTTP status code, and optionally the content-type.
     */
    void Status(Http::Status status_code);
    void Status(Http::Status status_code, std::string const& content_type);

    /**
     * Write content to the internal content buffer. Returns the length of content written.
     */
    size_t Write(std::string const&);

    /**
     * Get the content.
     */
    std::string Content() const;

   protected:
    std::unordered_map<std::string, std::string> headers;
    std::stringstream content;
};

}  // namespace bes::web

#endif
