#pragma once

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
    Route(Route&&) = default;
    Route(Route const&) = default;
    Route& operator=(Route&&) = default;
    Route& operator=(Route const&) = default;

    explicit Route(std::string const& name);
    Route(std::string const& name, std::string const& uri);

    void parseUri(std::string const& uri);

    std::string name;
    std::string controller;
    std::vector<RouteSection> parts;
    bool includes_query = false;

   private:
    static void trim(std::string& s);
};

struct PrecachedRoute : public Route
{
    PrecachedRoute() = default;
    PrecachedRoute(PrecachedRoute&&) = default;
    PrecachedRoute(PrecachedRoute const&) = default;
    PrecachedRoute& operator=(PrecachedRoute&&) = default;
    PrecachedRoute& operator=(PrecachedRoute const&) = default;

    explicit PrecachedRoute(Route&&);
    explicit PrecachedRoute(Route const&);
    PrecachedRoute& operator=(Route&&);
    PrecachedRoute& operator=(Route const&);

    std::string prefix;
    std::regex regex;
    std::vector<std::string> arg_map;

   private:
    void precache();
};

static std::string const regex_esc = "[\\^$.|?*+(){}";

}  // namespace bes::web
