#ifndef BES_WEB_ROUTE_H
#define BES_WEB_ROUTE_H

#include <regex>
#include <string>
#include <vector>

#include "exception.h"

namespace bes::web {

enum class RouteType : char
{
    LITERAL,
    REGEX,
};

struct RouteSection
{
    std::string value;
    std::string name;
    RouteType section_type;
};

struct Route
{
    Route() = default;
    Route(std::string const& name);
    Route(std::string const& name, std::string const& uri);

    void ParseUri(std::string const& uri);

    std::string name;
    std::string controller;
    std::vector<RouteSection> parts;
    bool includes_query = false;

   private:
    static void Trim(std::string& s);
};

struct PrecachedRoute : public Route
{
    PrecachedRoute() = default;

    PrecachedRoute(Route const& route);
    PrecachedRoute(Route&& route);
    PrecachedRoute& operator=(Route const& route);
    PrecachedRoute& operator=(Route&& route);

    PrecachedRoute(PrecachedRoute const& route) = default;
    PrecachedRoute(PrecachedRoute&& route);
    PrecachedRoute& operator=(PrecachedRoute route);
    PrecachedRoute& operator=(PrecachedRoute&& route);

    friend void swap(PrecachedRoute& first, PrecachedRoute& second) noexcept
    {
        using std::swap;
        swap(first.controller, second.controller);
        swap(first.name, second.name);
        swap(first.parts, second.parts);
        swap(first.includes_query, second.includes_query);
        swap(first.regex, second.regex);
        swap(first.prefix, second.prefix);
        swap(first.arg_map, second.arg_map);
    }

    std::string prefix;
    std::regex regex;
    std::vector<std::string> arg_map;

   private:
    void Precache();
};

static std::string const regex_esc = "[\\^$.|?*+(){}";

}  // namespace bes::web

#endif
