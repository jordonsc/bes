#include "http_request.h"

using namespace bes::web;

HttpRequest::HttpRequest(bes::fastcgi::Request const& base) : base_request(base)
{
    method = Http::MethodFromString(base_request.Param(Http::Parameter::REQUEST_METHOD));
    ParseQueryString();
    ParseCookies();
}

std::string const& HttpRequest::Uri() const
{
    return base_request.Param(Http::Parameter::DOCUMENT_URI);
}

std::string const& HttpRequest::QueryString() const
{
    return base_request.Param(Http::Parameter::QUERY_STRING);
}

Http::Method const& HttpRequest::Method() const
{
    return method;
}

/**
 * Parse the query string, breaking it down into key/value pairs and decoding URI encoding.
 */
void HttpRequest::ParseQueryString()
{
    std::string const& qs = QueryString();

    uint8_t mode = 0;
    unsigned char c;
    std::stringstream key;
    std::stringstream value;

    for (size_t i = 0; i <= qs.length(); ++i) {
        if (i == qs.length()) {
            c = '&';
        } else {
            c = qs[i];
        }

        if (c == '=' && mode == 0) {
            /// Switch to value mode
            ++mode = 1;
        } else if (c == '&') {
            /// Next item
            if (key.rdbuf()->in_avail()) {
                query_params[key.str()] = value.str();
            }
            key.str(std::string());
            value.str(std::string());
            mode = 0;
        } else {
            /// Parse normal character
            // Check for special encoding first -
            if (c == '+') {
                // + is URI for space
                c = ' ';
            } else if (c == '%' && (qs.length() - i > 2)) {
                // Convert % notation to a regular char, we must have at least 2 bytes remaining for this to be valid
                try {
                    c = (HexToChar(qs[i + 1]) * 16) + HexToChar(qs[i + 2]);
                    i += 2;
                } catch (std::out_of_range const& e) {
                    // Malformed URI, ignore the sequence
                }
            }

            // Add char to key/value
            if (mode == 0) {
                key << c;
            } else {
                value << c;
            }
        }
    }
}

void HttpRequest::ParseCookies()
{
    if (!base_request.HasParam("HTTP_COOKIE")) {
        return;
    }

    uint8_t mode = 0;
    std::stringstream key;
    std::stringstream value;

    for (char const& c : base_request.Param("HTTP_COOKIE")) {
        if (c == ';') {
            /// Next cookie
            if (key.rdbuf()->in_avail() && value.rdbuf()->in_avail()) {
                cookies.insert_or_assign(key.str(), value.str());
            }

            key.str(std::string());
            value.str(std::string());
            mode = 0;
            continue;

        } else if (mode == 0 && c == '=') {
            /// Move to value
            ++mode;
            continue;
        }

        if (mode == 0) {
            key << c;
        } else {
            value << c;
        }
    }

    if (key.rdbuf()->in_avail() && value.rdbuf()->in_avail()) {
        cookies.insert_or_assign(key.str(), value.str());
    }
}

/**
 * Converts an ASCII value of a hex character into the number it represents (eg 'D' == 13).
 */
char HttpRequest::HexToChar(char c)
{
    if (c >= 'a' && c <= 'z') {
        // Convert lowercase notation
        c -= 32;  // 32 is the ASCII difference from A to a
    } else if ((c < '0' || c > '9') && (c < 'A' || c > 'Z')) {
        // Not a hex character
        throw std::out_of_range(std::string("Char ") + c + " is not an hex character");
    }

    if (c > '9') {
        // 7 is the ASCII difference from 9 to A
        c -= '7';
    } else {
        c -= '0';
    }

    return c;
}

bool HttpRequest::HasQueryParam(std::string const& key) const
{
    return query_params.find(key) != query_params.end();
}

std::string const& HttpRequest::QueryParam(std::string const& key) const
{
    return query_params.at(key);
}

bool HttpRequest::HasCookie(std::string const& key) const
{
    return cookies.find(key) != cookies.end();
}

std::string const& HttpRequest::Cookie(std::string const& key) const
{
    return cookies.at(key);
}
