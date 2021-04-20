#pragma once

#define BES_CFG_KEY_ERR_MSG "Config key does not exist: "
#define BES_CFG_NO_FILE_ERR_MSG "Unable to load configuration file"

#include <bes/log.h>
#include <yaml-cpp/yaml.h>

#include "exception.h"
#include "file_finder.h"

namespace bes {

class Config
{
   public:
    Config() = default;

    void loadString(std::string const &source);

    /**
     * WILL throw a FileNotFoundException if this file cannot be read.
     */
    void loadFile(std::string const &source);

    /**
     * Will NOT throw if we can't find a file in the FileFinder, but will log a warning message.
     */
    void loadFile(bes::FileFinder const &ff);

    template <typename R, typename T, typename... Args>
    R get(T &&key, Args &&... args) const;

    /**
     * Same as Get() but returns a default value if the node doesn't exist or is a null-value.
     */
    template <typename R, typename T, typename... Args>
    R getOr(R const &_default, T &&key, Args &&... args) const;

    /**
     * Same as GetOr() but will raise a NullException if the node exists and is a null-value.
     */
    template <typename R, typename T, typename... Args>
    R getOrNull(R const &_default, T &&key, Args &&... args) const;

   protected:
    template <typename R, typename T, typename... Args>
    R traverseNested(YAML::Node const &base_node, T &&key, Args &&... args) const;

    template <typename R, typename T>
    R traverseNested(YAML::Node const &base_node, T &&key) const;

    template <typename T>
    YAML::Node traverseNested(YAML::Node const &base_node, T &&key) const;

    template <typename T>
    std::basic_string<char> traverseNested(YAML::Node const &base_node, T &&key) const;

    YAML::Node root;
};

template <typename R, typename T, typename... Args>
R bes::Config::get(T &&key, Args &&... args) const
{
    return traverseNested<R, T, Args...>(root, std::forward<T>(key), std::forward<Args>(args)...);
}

template <typename R, typename T, typename... Args>
R bes::Config::getOr(R const &_default, T &&key, Args &&... args) const
{
    try {
        return traverseNested<R, T, Args...>(root, std::forward<T>(key), std::forward<Args>(args)...);
    } catch (bes::IndexErrorException &) {
        return _default;
    }
}

template <typename R, typename T, typename... Args>
R bes::Config::getOrNull(R const &_default, T &&key, Args &&... args) const
{
    try {
        return traverseNested<R, T, Args...>(root, std::forward<T>(key), std::forward<Args>(args)...);
    } catch (bes::NullException const &e) {
        throw NullException(e);
    } catch (bes::IndexErrorException const &) {
        return _default;
    }
}

template <typename R, typename T, typename... Args>
R bes::Config::traverseNested(YAML::Node const &base_node, T &&key, Args &&... args) const
{
    YAML::Node n = base_node[std::forward<T>(key)];
    if (!n.IsDefined()) {
        throw bes::IndexErrorException(std::string(BES_CFG_KEY_ERR_MSG) + key);
    }

    return traverseNested<R, Args...>(n, std::forward<Args>(args)...);
}

template <typename R, typename T>
R bes::Config::traverseNested(YAML::Node const &base_node, T &&key) const
{
    YAML::Node n = base_node[std::forward<T>(key)];
    if (!n.IsDefined()) {
        throw bes::IndexErrorException(std::string(BES_CFG_KEY_ERR_MSG) + key);
    }

    if (n.IsNull()) {
        throw bes::NullException(key);
    } else {
        return n.as<R>();
    }
}

template <typename T>
YAML::Node bes::Config::traverseNested(YAML::Node const &base_node, T &&key) const
{
    return base_node[std::forward<T>(key)];
}

}  // namespace bes
