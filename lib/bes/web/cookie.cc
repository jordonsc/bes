#include "cookie.h"

#include "bes/web/exception.h"

using namespace bes::web;

Cookie::Cookie(std::string name, std::string value) : name(std::move(name)), value(std::move(value))
{
    if (!isValid(name, true)) {
        throw WebException("Cookie name is not valid: " + name);
    }

    if (!isValid(value, false)) {
        throw WebException("Cookie value for '" + name + "' is not valid: " + value);
    }

    expires = std::chrono::system_clock::now();
}

std::string const& Cookie::getName() const
{
    return name;
}

std::string const& Cookie::getValue() const
{
    return value;
}

Cookie& Cookie::setDomain(std::string d)
{
    domain = std::move(d);
    return *this;
}

std::string const& Cookie::getDomain() const
{
    return domain;
}

Cookie& Cookie::setPath(std::string p)
{
    path = std::move(p);
    return *this;
}

std::string const& Cookie::getPath() const
{
    return path;
}

Cookie& Cookie::setMaxAge(int64_t age)
{
    max_age = age;
    expires = std::chrono::system_clock::now();
    return *this;
}

int64_t Cookie::getMaxAge() const
{
    return max_age;
}

Cookie& Cookie::setExpires(std::chrono::system_clock::time_point exp)
{
    expires = exp;
    max_age = 0;
    return *this;
}

std::chrono::system_clock::time_point Cookie::getExpires() const
{
    return expires;
}

Cookie& Cookie::setSecure(bool v)
{
    secure = v;
    return *this;
}

bool Cookie::isSecure() const
{
    return secure;
}

Cookie& Cookie::setHttpOnly(bool v)
{
    http_only = v;
    return *this;
}

bool Cookie::isHttpOnly() const
{
    return http_only;
}

Cookie& Cookie::setPriority(Cookie::CookiePriority p)
{
    priority = p;
    return *this;
}

Cookie::CookiePriority Cookie::getPriority() const
{
    return priority;
}

/**
 * Check is a value is cookie-safe.
 *
 * If `is_name` is true, additionally checks for = char which is not name-safe.
 */
bool Cookie::isValid(std::string const& str, bool is_name)
{
    return std::all_of(str.begin(), str.end(), [is_name](char const c) {
        return !(c < '!' || c > '~' || c == ',' || c == ';' || (is_name && c == '='));
    });
}
