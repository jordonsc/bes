#ifndef BES_WEB_EXCEPTION_H
#define BES_WEB_EXCEPTION_H

#include <utility>

#include <bes/core.h>
#include <bes/fastcgi.h>
#include "http.h"

namespace bes::web {

class WebException : public bes::BesException
{
    using BesException::BesException;
};

class MalformedExpressionException : public WebException
{
    using WebException::WebException;
};

class MissingArgumentException : public WebException
{
    using WebException::WebException;
};

class NoRouteException : public WebException
{
    using WebException::WebException;
};

class SessionNotExistsException : public WebException
{
    using WebException::WebException;
};

class SessionIndexError : public WebException
{
    using WebException::WebException;
};

class NoMatchException : public WebException
{
   public:
    NoMatchException() : WebException("No match") {}
    explicit NoMatchException(std::string const& str) : WebException(str) {}
};

/**
 * Used when a Router cannot yield a controller for this request.
 */
class CannotYieldException : public WebException
{
   public:
    CannotYieldException() : WebException("Cannot yield response"){};
};

/**
 * HttpWebExceptions represent a collection of HTTP errors that can be handled in intentional ways by the webserver.
 * This includes redirects, not-found pages, etc.
 */
class HttpWebException : public WebException
{
   public:
    HttpWebException(Http::Status http_code, std::string const& string)
        : WebException(string, bes::ExitCode::GEN_ERR), http_code(http_code)
    {}

    [[nodiscard]] Http::Status HttpCode() const
    {
        return http_code;
    }

   protected:
    Http::Status http_code;
};

// All of the following exceptions are helpers for the above exception. By all means, if you want to throw a status code
// not listed below, simply throw the HttpWebException with the appropriate response code and it will respond in the
// correct manner.

// --- 300 Family Exceptions --- ///

class RedirectHttpException : public HttpWebException
{
   public:
    RedirectHttpException(Http::Status http_code, std::string const& msg, std::string target)
        : HttpWebException(http_code, msg), tgt(std::move(target))
    {}

    [[nodiscard]] std::string const& Target() const
    {
        return tgt;
    }

   protected:
    std::string tgt;
};

/// Process flow redirect - 303
class SystemRedirectHttpException : public RedirectHttpException
{
   public:
    explicit SystemRedirectHttpException(std::string const& tgt)
        : RedirectHttpException(Http::Status::SEE_OTHER, "See Other", tgt)
    {}
};

/// Temp redirect - 307
class TemporaryRedirectHttpException : public RedirectHttpException
{
   public:
    explicit TemporaryRedirectHttpException(std::string const& tgt)
        : RedirectHttpException(Http::Status::TEMPORARY_REDIRECT, "Temporary redirect", tgt)
    {}
};

/// Perm redirect - 308
class PermanentRedirectHttpException : public RedirectHttpException
{
   public:
    explicit PermanentRedirectHttpException(std::string const& tgt)
        : RedirectHttpException(Http::Status::PERMANENT_REDIRECT, "Permanent redirect", tgt)
    {}
};

// --- 400 Family Exceptions --- ///
/// Unauthorised - 401
class UnauthorisedHttpException : public HttpWebException
{
   public:
    UnauthorisedHttpException() : HttpWebException(Http::Status::UNAUTHORIZED, "Unauthorised") {}
};

/// Page Not Found - 404
class NotFoundHttpException : public HttpWebException
{
   public:
    NotFoundHttpException() : HttpWebException(Http::Status::NOT_FOUND, "Page Not Found") {}
};

// --- 500 Family Exceptions ---
/// Internal Server Error - 500
class InternalServerErrorHttpException : public HttpWebException
{
   public:
    explicit InternalServerErrorHttpException(std::string const& msg)
        : HttpWebException(Http::Status::INTERNAL_SERVER_ERROR, msg)
    {}
};

};  // namespace bes::web

#endif
