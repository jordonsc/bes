#pragma once

#include <chrono>
#include <string>

namespace bes::web {

class Cookie
{
   public:
    enum class CookiePriority : char
    {
        NONE,
        LOW,
        MEDIUM,
        HIGH
    };

    Cookie(std::string name, std::string value);
    Cookie(Cookie const&) = default;
    Cookie(Cookie&&) = default;
    Cookie& operator=(Cookie const&) = default;
    Cookie& operator=(Cookie&&) = default;

    [[nodiscard]] std::string const& getName() const;
    [[nodiscard]] std::string const& getValue() const;

    Cookie& setDomain(std::string d);
    [[nodiscard]] std::string const& getDomain() const;

    Cookie& setPath(std::string p);
    [[nodiscard]] std::string const& getPath() const;

    Cookie& setMaxAge(int64_t age);
    [[nodiscard]] int64_t getMaxAge() const;

    /**
     * We control whether this header is used by determining if it's in the past. This prevents us from creating
     * delete-cookies, which are defined by an Expires field in the past.
     *
     * TODO: Add a feature to create delete-cookies.
     */
    Cookie& setExpires(std::chrono::system_clock::time_point exp);
    [[nodiscard]] std::chrono::system_clock::time_point getExpires() const;

    Cookie& setSecure(bool v);
    [[nodiscard]] bool isSecure() const;

    Cookie& setHttpOnly(bool v);
    [[nodiscard]] bool isHttpOnly() const;

    Cookie& setPriority(CookiePriority p);
    [[nodiscard]] CookiePriority getPriority() const;

   protected:
    std::string name;
    std::string value;

    std::string domain;
    std::string path;

    uint64_t max_age = 0;
    std::chrono::system_clock::time_point expires;

    bool secure = false;
    bool http_only = false;

    // Limited support:
    CookiePriority priority = CookiePriority::NONE;

    static bool isValid(std::string const& str, bool is_name);
};

}  // namespace bes::web

