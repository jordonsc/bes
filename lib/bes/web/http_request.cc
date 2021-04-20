#include "http_request.h"

using namespace bes::web;

HttpRequest::HttpRequest(bes::fastcgi::Request const& base, std::string const& session_prefix) : base_request(base)
{
    http_method = Http::methodFromString(base_request.getParam(Http::Parameter::REQUEST_METHOD));
    parseQueryString();
    parseCookies();
    bootstrapSession(session_prefix);
}

HttpRequest::~HttpRequest()
{
    // Persist the session
    if (hasSession()) {
        auto session_mgr = base_request.container.get<bes::web::SessionInterface>(SVC_SESSION_MGR);
        if (session_mgr != nullptr) {
            session_mgr->persistSession(session);
        }
    }
}

std::string const& HttpRequest::uri() const
{
    return base_request.getParam(Http::Parameter::DOCUMENT_URI);
}

std::string const& HttpRequest::queryString() const
{
    return base_request.getParam(Http::Parameter::QUERY_STRING);
}

Http::Method const& HttpRequest::method() const
{
    return http_method;
}

/**
 * Parse the query string, breaking it down into key/value pairs and decoding URI encoding.
 */
void HttpRequest::parseQueryString()
{
    std::string const& qs = queryString();

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
                    c = (hexToChar(qs[i + 1]) * 16) + hexToChar(qs[i + 2]);
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

void HttpRequest::parseCookies()
{
    if (!base_request.hasParam("HTTP_COOKIE")) {
        return;
    }

    uint8_t mode = 0;
    std::stringstream key;
    std::stringstream value;

    for (char const& c : base_request.getParam("HTTP_COOKIE")) {
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
        } else if (std::isspace(c)) {
            // Whitespace is not a legal cookie character, we'll use this opportunity to trim whitespace around the
            // header parameters.
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
char HttpRequest::hexToChar(char c)
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

bool HttpRequest::hasQueryParam(std::string const& key) const
{
    return query_params.find(key) != query_params.end();
}

std::string const& HttpRequest::queryParam(std::string const& key) const
{
    return query_params.at(key);
}

bool HttpRequest::hasCookie(std::string const& key) const
{
    return cookies.find(key) != cookies.end();
}

std::string const& HttpRequest::getCookie(std::string const& key) const
{
    return cookies.at(key);
}

bool HttpRequest::hasSession() const
{
    return !session.sessionId().empty();
}

Session& HttpRequest::getSession() const
{
    if (!hasSession()) {
        // Create a new session
        auto session_mgr = base_request.container.get<bes::web::SessionInterface>(SVC_SESSION_MGR);
        if (session_mgr == nullptr) {
            BES_LOG(WARNING) << "Session requested but no session manager available";
            return session;
        }

        session = session_mgr->createSession(*(base_request.container.get<std::string>(SESSION_PREFIX_KEY)));
    }

    return session;
}

/**
 * Create a session object if a session cookie exists.
 *
 * This function must be called after the cookies has been initialised, as it depends on them.
 */
void HttpRequest::bootstrapSession(std::string const& prefix)
{
    auto session_mgr = base_request.container.get<bes::web::SessionInterface>(SVC_SESSION_MGR);
    if (session_mgr == nullptr) {
        return;
    }

    auto session_cookie = base_request.container.get<std::string>(SESSION_COOKIE_KEY);

    if (hasCookie(*session_cookie)) {
        // Session cookie exists, query manager for it
        try {
            session = session_mgr->getSession(getCookie(*session_cookie));
        } catch (SessionNotExistsException const&) {
            // Session has likely expired, create a new one
            session = session_mgr->createSession(prefix);
        }
    }
}

bool HttpRequest::hasParam(std::string const& key) const
{
    return base_request.hasParam(key);
}

std::string const& HttpRequest::getParam(std::string const& key) const
{
    return base_request.getParam(key);
}
