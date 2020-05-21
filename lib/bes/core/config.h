#ifndef BES_FILESYSTEM_CONFIG_H
#define BES_FILESYSTEM_CONFIG_H

#define BES_CFG_KEY_ERR_MSG "Config key does not exist: "
#define BES_CFG_NO_FILE_ERR_MSG "Unable to load configuration file"

#include <yaml-cpp/yaml.h>

#include "bes/log.h"
#include "exception.h"
#include "file_finder.h"

namespace bes {

class Config
{
   public:
    Config() = default;

    void LoadString(std::string const &);

    /**
     * WILL throw a FileNotFoundException if this file cannot be read.
     */
    void LoadFile(std::string const &);

    /**
     * Will NOT throw if we can't find a file in the FileFinder, but will log a warning message.
     */
    void LoadFile(bes::FileFinder const &);

    template <typename R, typename T, typename... Args>
    R Get(T &&key, Args &&... args) const;

    /**
     * Same as Get() but returns a default value if the node doesn't exist or is a null-value.
     */
    template <typename R, typename T, typename... Args>
    R GetOr(R const &_default, T &&key, Args &&... args) const;

    /**
     * Same as GetOr() but will raise a NullException if the node exists and is a null-value.
     */
    template <typename R, typename T, typename... Args>
    R GetOrNull(R const &_default, T &&key, Args &&... args) const;

   protected:
    template <typename R, typename T, typename... Args>
    R TraverseNested(YAML::Node const &, T &&key, Args &&... args) const;

    template <typename R, typename T>
    R TraverseNested(YAML::Node const &, T &&key) const;

    template <typename T>
    YAML::Node TraverseNested(YAML::Node const &, T &&key) const;

    template <typename T>
    std::basic_string<char> TraverseNested(YAML::Node const &, T &&key) const;

    YAML::Node root;
};

template <typename R, typename T, typename... Args>
R bes::Config::Get(T &&key, Args &&... args) const
{
    return TraverseNested<R, T, Args...>(root, std::forward<T>(key), std::forward<Args>(args)...);
}

template <typename R, typename T, typename... Args>
R bes::Config::GetOr(R const &_default, T &&key, Args &&... args) const
{
    try {
        return TraverseNested<R, T, Args...>(root, std::forward<T>(key), std::forward<Args>(args)...);
    } catch (bes::IndexErrorException &) {
        return _default;
    }
}

template <typename R, typename T, typename... Args>
R bes::Config::GetOrNull(R const &_default, T &&key, Args &&... args) const
{
    try {
        return TraverseNested<R, T, Args...>(root, std::forward<T>(key), std::forward<Args>(args)...);
    } catch (bes::NullException &e) {
        throw e;
    } catch (bes::IndexErrorException &) {
        return _default;
    }
}

template <typename R, typename T, typename... Args>
R bes::Config::TraverseNested(YAML::Node const &base_node, T &&key, Args &&... args) const
{
    YAML::Node n = base_node[std::forward<T>(key)];
    if (!n.IsDefined()) {
        throw bes::IndexErrorException(std::string(BES_CFG_KEY_ERR_MSG) + key);
    }

    return TraverseNested<R, Args...>(n, std::forward<Args>(args)...);
}

template <typename R, typename T>
R bes::Config::TraverseNested(YAML::Node const &base_node, T &&key) const
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
YAML::Node bes::Config::TraverseNested(YAML::Node const &base_node, T &&key) const
{
    return base_node[std::forward<T>(key)];
}

}  // namespace bes

#endif
