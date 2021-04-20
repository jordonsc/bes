#pragma once

#include <yaml-cpp/yaml.h>

#include <functional>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "route.h"
#include "router.h"

namespace bes::web {

class MappedRouter : public Router
{
   public:
    HttpResponse yieldResponse(HttpRequest const& request) const override;
    HttpResponse yieldErrorResponse(HttpRequest const& request, Http::Status status_code,
                                    std::string const& debug_msg) const override;

    void loadRoutesFromString(std::string const& src);
    void loadRoutesFromFile(std::string const& fn);

    void addRoute(Route route);

    /**
     * Get a string URI from a route name (and args, if the route requires them).
     */
    std::string getUri(std::string const& route_name) const;
    std::string getUri(std::string const& route_name, ActionArgs const& args) const;

    /**
     * Register a control with this router.
     *
     * Can be in the form of a lambda or a member function of a class object, consider:
     *   ctrl = MyController();
     *   router.RegisterController("home", &MyController::SomeAction, &ctrl);
     */
    void registerController(std::string const& name, Controller c);
    template <class MethodT, class ObjT>
    void registerController(std::string const& name, MethodT&& method, ObjT&& object);

    /**
     * Returns a map of all registered routes.
     */
    std::unordered_map<std::string, PrecachedRoute> const& routeMap();

    /**
     * Match a route to the given URI.
     *
     * Throws a NoMatchException if no route matches the URI.
     */
    std::tuple<Route const&, ActionArgs> findRoute(std::string const& uri,
                                                   std::string const& query = std::string()) const;

    size_t inline size() const
    {
        return routes.size();
    }

   protected:
    std::unordered_map<std::string, Controller> controllers;
    std::unordered_map<std::string, PrecachedRoute> routes;

   private:
    void parseRoutes(YAML::Node& root);

    static ActionArgs routeMatch(PrecachedRoute const& route, std::string const& uri);

    template <class T>
    static T getNodeValue(YAML::Node const& node, std::string const& key, T default_value);
};

template <class MethodT, class ObjT>
inline void MappedRouter::registerController(std::string const& name, MethodT&& method, ObjT&& object)
{
    using namespace std::placeholders;

    if (controllers.find(name) != controllers.end()) {
        throw WebException("Controller '" + name + "' already exists");
    }

    controllers[name] = std::bind(std::forward<MethodT>(method), std::forward<ObjT>(object), _1, _2);
}

template <class T>
inline T MappedRouter::getNodeValue(YAML::Node const& node, std::string const& key, T default_value)
{
    try {
        auto sub_node = node[key];
        if (sub_node.IsDefined() && !sub_node.IsNull()) {
            return sub_node.as<T>();
        } else {
            return default_value;
        }
    } catch (...) {
        return default_value;
    }
}

}  // namespace bes::web
