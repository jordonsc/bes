#ifndef BES_WEB_COOKIE_H
#define BES_WEB_COOKIE_H

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

    [[nodiscard]] std::string const& Name() const;
    [[nodiscard]] std::string const& Value() const;

    Cookie& Domain(std::string domain);
    [[nodiscard]] std::string const& Domain() const;

    Cookie& Path(std::string path);
    [[nodiscard]] std::string const& Path() const;

    Cookie& MaxAge(int64_t age);
    [[nodiscard]] int64_t MaxAge() const;

    /**
     * We control whether this header is used by determining if it's in the past. This prevents us from creating
     * delete-cookies, which are defined by an Expires field in the past.
     *
     * TODO: Add a feature to create delete-cookies.
     */
    Cookie& Expires(std::chrono::system_clock::time_point exp);
    [[nodiscard]] std::chrono::system_clock::time_point Expires() const;

    Cookie& Secure(bool v);
    [[nodiscard]] bool Secure() const;

    Cookie& HttpOnly(bool v);
    [[nodiscard]] bool HttpOnly() const;

    Cookie& Priority(CookiePriority p);
    [[nodiscard]] CookiePriority Priority() const;

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

    static bool IsValid(std::string const& str, bool is_name);
};

}  // namespace bes::web

#endif
