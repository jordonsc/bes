#pragma once

#include <unordered_map>

namespace bes::web {

struct Http
{
    enum class Status : int
    {
        // 1xx informational response
        CONTINUE = 100,
        SWITCHING_PROTOCOLS = 101,
        PROCESSING = 102,
        EARLY_HINTS = 103,

        // 2xx success
        OK = 200,
        CREATED = 201,
        ACCEPTED = 202,
        NON_AUTHORITATIVE_INFORMATION = 203,
        NO_CONTENT = 204,
        RESET_CONTENT = 205,
        PARTIAL_CONTENT = 206,
        MULTI_STATUS = 207,
        ALREADY_REPORTED = 208,
        IM_USED = 209,

        // 3xx redirect
        MULTIPLE_CHOICES = 300,
        MOVED_PERMANENTLY = 301,
        FOUND = 302,
        SEE_OTHER = 303,
        NOT_MODIFIED = 304,
        USE_PROXY = 305,
        SWITCH_PROXY = 306,
        TEMPORARY_REDIRECT = 307,
        PERMANENT_REDIRECT = 308,

        // 4xx client error
        BAD_REQUEST = 400,
        UNAUTHORIZED = 401,
        PAYMENT_REQUIRED = 402,
        FORBIDDEN = 403,
        NOT_FOUND = 404,
        METHOD_NOT_ALLOWED = 405,
        NOT_ACCEPTABLE = 406,
        PROXY_AUTHENTICATION_REQUIRED = 407,
        REQUEST_TIMEOUT = 408,
        CONFLICT = 409,
        GONE = 410,
        // TODO: complete the 4xx codes

        // 5xx server error
        INTERNAL_SERVER_ERROR = 500,
        NOT_IMPLEMENTED = 501,
        BAD_GATEWAY = 502,
        SERVICE_UNAVAILABLE = 503,
        GATEWAY_TIMEOUT = 504,
        HTTP_VERSION_NOT_SUPPORTED = 505,
        // TODO: complete the 5xx codes

    };

    struct Header
    {
        constexpr const static auto STATUS = "Status";
        constexpr const static auto CONTENT_TYPE = "Content-Type";
        constexpr const static auto CONTENT_LENGTH = "Content-Length";
        constexpr const static auto CONTENT_DISPOSITION = "Content-Disposition";
        constexpr const static auto LOCATION = "Location";
    };

    struct Parameter
    {
        constexpr const static auto REQUEST_METHOD = "REQUEST_METHOD";
        constexpr const static auto DOCUMENT_URI = "DOCUMENT_URI";
        constexpr const static auto REQUEST_URI = "REQUEST_URI";
        constexpr const static auto QUERY_STRING = "QUERY_STRING";
        constexpr const static auto REMOTE_ADDR = "REMOTE_ADDR";
    };

    struct ContentType
    {
        constexpr const static auto HTML = "text/html";
        constexpr const static auto TEXT = "text/plain";
        constexpr const static auto JSON = "application/json";
        constexpr const static auto BINARY = "application/octet-stream";
    };

    enum class Method
    {
        GET,
        POST,
        PUT,
        DELETE,
        HEAD,
        OPTIONS,
        TRACE,
        CONNECT
    };

    static Method methodFromString(std::string const& s)
    {
        if (s == "GET") {
            return Method::GET;
        } else if (s == "POST") {
            return Method::POST;
        } else if (s == "PUT") {
            return Method::PUT;
        } else if (s == "DELETE") {
            return Method::DELETE;
        } else if (s == "HEAD") {
            return Method::HEAD;
        } else if (s == "OPTIONS") {
            return Method::OPTIONS;
        } else if (s == "TRACE") {
            return Method::TRACE;
        } else if (s == "CONNECT") {
            return Method::CONNECT;
        } else {
            throw std::runtime_error("Unknown HTTP method: " + s);
        }
    }
};

using ActionArgs = std::unordered_map<std::string, std::string>;

}  // namespace bes::web
