#include "route.h"

using namespace bes::web;

Route::Route(std::string const& name) : name(name), controller(name) {}

Route::Route(std::string const& name, std::string const& uri) : name(name), controller(name)
{
    parseUri(uri);
}

/**
 * Parse the route URI string and break it into segments for matching.
 */
void Route::parseUri(std::string const& uri)
{
    std::stringstream part;
    uint16_t depth = 0;
    char mode = 0;
    RouteSection sect;

    for (char const c : uri) {
        if (c == '{' && !depth++) {
            // Outside an arg block, save current segment and open a new one
            sect.section_type = RouteType::LITERAL;
            sect.name = "";
            sect.value = part.str();

            if (sect.value.length()) {
                parts.push_back(sect);
                part.str(std::string());
            }

            mode = 0;
            continue;
        } else if (c == '}' && depth > 0 && --depth == 0) {
            // Finish up the arg block
            if (mode == 0) {
                sect.name = part.str();
                sect.value = ".+?";
            } else {
                sect.value = part.str();
            }
            sect.section_type = RouteType::REGEX;

            trim(sect.name);
            trim(sect.value);

            if (sect.name.length() && sect.value.length()) {
                parts.push_back(sect);
            }

            part.str(std::string());
            mode = 0;
            continue;
        } else if (depth && mode == 0 && c == ':') {
            sect.name = part.str();
            part.str(std::string());
            ++mode;
            continue;
        }

        part << c;
    }

    // We should have a zero-depth, as { .. } blocks should have closed before the end of the string
    if (depth) {
        throw MalformedExpressionException("Route '" + name + "' URI of '" + uri + "' is malformed");
    }

    // Add whatever is left as a literal value
    sect.section_type = RouteType::LITERAL;
    sect.name = "";
    sect.value = part.str();

    if (sect.value.length()) {
        parts.push_back(sect);
        part.str(std::string());
    }
}

void Route::trim(std::string& s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
                return !std::isspace(ch);
            }));

    s.erase(std::find_if(s.rbegin(), s.rend(),
                         [](int ch) {
                             return !std::isspace(ch);
                         })
                .base(),
            s.end());
}

void PrecachedRoute::precache()
{
    std::stringstream buf;  // Regex builder
    bool first = true;      // Is first part

    prefix = "";
    arg_map.clear();

    buf << '^';

    for (auto const& part : parts) {
        // We'll do a string match on the first part (which _should_ always be a literal, but isn't mandatory) in order
        // to minimise the about of regex matching we're doing.
        if (first && part.section_type == RouteType::LITERAL) {
            prefix = parts[0].value;
            first = false;
            continue;
        }
        first = false;

        switch (part.section_type) {
            case RouteType::LITERAL:
                for (char c : part.value) {
                    if (regex_esc.find(c) == std::string::npos) {
                        buf << c;
                    } else {
                        buf << '\\' << c;
                    }
                }
                break;
            case RouteType::REGEX:
                arg_map.push_back(part.name);
                buf << '(' << part.value << ')';
                break;
            default:
                throw std::runtime_error("Unimplemented route type for route: " + name);
        }
    }

    buf << '$';
    std::string route_re_str = buf.str();

    if (route_re_str.length() > 2) {
        regex = route_re_str;
    }
}

PrecachedRoute::PrecachedRoute(Route&& r) : Route(std::move(r))
{
    precache();
}

PrecachedRoute::PrecachedRoute(Route const& r) : Route(r)
{
    precache();
}

PrecachedRoute& PrecachedRoute::operator=(Route&& r)
{
    auto tmp = PrecachedRoute(std::move(r));
    std::swap(*this, tmp);
    precache();

    return *this;
}

PrecachedRoute& PrecachedRoute::operator=(Route const& r)
{
    auto tmp = PrecachedRoute(r);
    std::swap(*this, tmp);
    precache();

    return *this;
}
