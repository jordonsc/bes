#include "cookie.h"

#include "bes/web/exception.h"

using namespace bes::web;

Cookie::Cookie(std::string name, std::string value) : name(std::move(name)), value(std::move(value))
{
    if (!IsValid(name, true)) {
        throw WebException("Cookie name is not valid: " + name);
    }

    if (!IsValid(value, false)) {
        throw WebException("Cookie value for '" + name + "' is not valid: " + value);
    }

    expires = std::chrono::system_clock::now();
}

std::string const& Cookie::Name() const
{
    return name;
}

std::string const& Cookie::Value() const
{
    return value;
}

Cookie& Cookie::Domain(std::string d)
{
    domain = std::move(d);
    return *this;
}

std::string const& Cookie::Domain() const
{
    return domain;
}

Cookie& Cookie::Path(std::string p)
{
    path = std::move(p);
    return *this;
}

std::string const& Cookie::Path() const
{
    return path;
}

Cookie& Cookie::MaxAge(int64_t age)
{
    max_age = age;
    expires = std::chrono::system_clock::now();
    return *this;
}

int64_t Cookie::MaxAge() const
{
    return max_age;
}

Cookie& Cookie::Expires(std::chrono::system_clock::time_point exp)
{
    expires = exp;
    max_age = 0;
    return *this;
}

std::chrono::system_clock::time_point Cookie::Expires() const
{
    return expires;
}

Cookie& Cookie::Secure(bool v)
{
    secure = v;
    return *this;
}

bool Cookie::Secure() const
{
    return secure;
}

Cookie& Cookie::HttpOnly(bool v)
{
    http_only = v;
    return *this;
}

bool Cookie::HttpOnly() const
{
    return http_only;
}

Cookie& Cookie::Priority(Cookie::CookiePriority p)
{
    priority = p;
    return *this;
}

Cookie::CookiePriority Cookie::Priority() const
{
    return priority;
}

/**
 * Check is a value is cookie-safe.
 *
 * If `is_name` is true, additionally checks for = char which is not name-safe.
 */
bool Cookie::IsValid(std::string const& str, bool is_name)
{
    for (char const& c : str) {
        if (c < '!' || c > '~' || c == ',' || c == ';' || (is_name && c == '=')) {
            return false;
        }
    }
    return true;
}
