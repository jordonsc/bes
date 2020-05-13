#include "mapped_router.h"

using namespace bes::web;

void MappedRouter::RegisterController(std::string const& name, Controller c)
{
    if (controllers.find(name) != controllers.end()) {
        throw WebException("Controller '" + name + "' already exists");
    }

    controllers[name] = c;
}

void MappedRouter::LoadRoutesFromString(std::string const& src)
{
    YAML::Node root = YAML::Load(src);
    ParseRoutes(root);
}

void MappedRouter::LoadRoutesFromFile(std::string const& fn)
{
    YAML::Node root = YAML::LoadFile(fn);
    ParseRoutes(root);
}

void MappedRouter::AddRoute(Route const& route)
{
    BES_LOG(DEBUG) << "Registered route: " << route.name;
    routes[route.name] = route;
}

void MappedRouter::AddRoute(Route&& route)
{
    BES_LOG(DEBUG) << "Registered route: " << route.name;
    routes[route.name] = std::forward<Route>(route);
}

std::unordered_map<std::string, PrecachedRoute> const& MappedRouter::RouteMap()
{
    return routes;
}

void MappedRouter::ParseRoutes(YAML::Node& root)
{
    for (auto node : root) {
        std::string name;
        try {
            name = node.first.as<std::string>();
        } catch (std::exception const& e) {
            BES_LOG(ERROR) << "Error parsing route map, bad node: " << e.what();
        }

        try {
            std::string uri = GetNodeValue(node.second, "uri", std::string());
            if (!uri.length()) {
                BES_LOG(ERROR) << "Route '" << name << "' has no URI";
                continue;
            }

            if (uri[0] != '/') {
                BES_LOG(WARNING) << "WARNING: route '" << name << "' does not start with a leading /";
            }

            bes::web::Route route(name, uri);
            route.includes_query = GetNodeValue(node.second, "includes_query", false);
            route.controller = GetNodeValue(node.second, "controller", route.name);

            BES_LOG(DEBUG) << "Registered route: " << route.name;
            routes[route.name] = std::move(route);

        } catch (std::exception const& e) {
            BES_LOG(ERROR) << "Error parsing route '" << name << "': " << e.what();
        }
    }
}

std::string MappedRouter::GetUri(std::string const& route_name) const
{
    auto const& route = routes.find(route_name);
    if (route == routes.end()) {
        throw NoRouteException("Route '" + route_name + "' does not exist");
    }

    if (route->second.parts.size() != 1 || route->second.parts[0].section_type != RouteType::LITERAL) {
        throw MissingArgumentException("Route '" + route_name + "' requires an argument list");
    }

    return route->second.parts[0].value;
}

std::string MappedRouter::GetUri(std::string const& route_name, ActionArgs const& args) const
{
    auto const& route = routes.find(route_name);
    if (route == routes.end()) {
        throw NoRouteException("Route '" + route_name + "' does not exist");
    }

    std::stringstream buf;

    for (auto const& part : route->second.parts) {
        switch (part.section_type) {
            case RouteType::LITERAL:
                // Literal, can render directly
                buf << part.value;
                break;
            case RouteType::REGEX:
                // Regex part, pull from arg list
                {
                    auto arg = args.find(part.name);
                    if (arg == args.end()) {
                        throw MissingArgumentException("Missing argument '" + part.name + "' for route '" + route_name +
                                                       "'");
                    }
                    buf << arg->second;
                    break;
                }
            default:
                throw std::runtime_error("Unimplemented route type for route: " + route_name);
        }
    }

    return buf.str();
}

HttpResponse MappedRouter::YieldResponse(HttpRequest const& request) const
{
    try {
        auto [route, args] = FindRoute(request.Uri(), request.QueryString());

        auto const& ctrl = controllers.find(route.controller);
        if (ctrl == controllers.end()) {
            throw InternalServerErrorHttpException("Route '" + route.name + "' requested missing controller '" +
                                                   route.controller + "'");
        }

        return ctrl->second(request, args);

    } catch (NoMatchException const&) {
        throw CannotYieldException();
    }
}

/**
 * Error router.
 *
 * Assumes a controller named "error_xxx" or "error", where "xxx" is the HTTP status code (such as 404). Will throw a
 * CannotYieldException if neither controllers exist.
 *
 * Injects the following into the ActionArgs:
 *  - error_code: 404
 *  - error_title: Page not found
 *  - debug_msg: Details of the error; do not render unless in debug mode
 *
 * Will set the correct HTTP status code to match the error, regardless of what the controller tries to return.
 */
HttpResponse MappedRouter::YieldErrorResponse(HttpRequest const& request, Http::Status status_code,
                                              std::string const& debug_msg) const
{
    bes::web::ActionArgs args;

    args["error_code"] = std::to_string(static_cast<int>(status_code));
    args["debug_msg"] = debug_msg;

    switch (status_code) {
        case bes::web::Http::Status::NOT_FOUND:
            args["error_title"] = "Page not found";
            break;
        case bes::web::Http::Status::UNAUTHORIZED:
        case bes::web::Http::Status::FORBIDDEN:
            args["error_title"] = "Access denied";
            break;
        default:
            int code = static_cast<int>(status_code);
            if (code >= 400 && code < 500) {
                args["error_title"] = "Bad request";
            } else {
                args["error_title"] = "Internal error";
            }
            break;
    }

    // First look for a specialised error controller
    auto it = controllers.find("error_" + args["error_code"]);
    if (it == controllers.end()) {
        // Then look for a generalised error template
        it = controllers.find("error");
        if (it == controllers.end()) {
            throw CannotYieldException();
        }
    }

    auto resp = it->second(request, args);

    // Force the correct status code on the response
    resp.Status(status_code);
    return resp;
}

std::tuple<Route const&, ActionArgs> MappedRouter::FindRoute(std::string const& uri, std::string const& query) const
{
    for (auto const& it : routes) {
        try {
            if (it.second.includes_query && query.length()) {
                return {it.second, RouteMatch(it.second, uri + "?" + query)};
            } else {
                return {it.second, RouteMatch(it.second, uri)};
            }
        } catch (NoMatchException const&) {
        }
    }

    throw NoMatchException("No route for URI '" + uri + "'");
}

ActionArgs MappedRouter::RouteMatch(PrecachedRoute const& route, std::string const& uri)
{
    ActionArgs args;

    size_t start_len = route.prefix.length();
    if (start_len && uri.substr(0, start_len) != route.prefix) {
        throw NoMatchException();
    }

    if (!route.arg_map.size()) {
        // Has no regex, start_len should match the length of the URI
        if (start_len == uri.length()) {
            return args;
        } else {
            throw NoMatchException();
        }
    }

    std::smatch match;
    std::string uri_re_part = uri.substr(start_len);

    if (std::regex_match(uri_re_part, match, route.regex) && match.size() == route.arg_map.size() + 1) {
        for (uint16_t i = 0; i < route.arg_map.size(); ++i) {
            args[route.arg_map[i]] = match[i + 1].str();
        }
        return args;
    } else {
        throw NoMatchException();
    }
}
