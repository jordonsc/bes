#ifndef BES_FILESYSTEM_FILEFINDER_H
#define BES_FILESYSTEM_FILEFINDER_H

#include <filesystem>
#include <vector>

#include "lib/log/log.h"

namespace bes {

using filesystem_t = std::string;

class FileFinder
{
   public:
    explicit inline FileFinder(bool require_read = true, bool require_write = false)
        : req_read(require_read), req_write(require_write)
    {}

    explicit inline FileFinder(std::vector<filesystem_t> const& search_paths, bool require_read = true,
                               bool require_write = false)
        : search_path(search_paths), req_read(require_read), req_write(require_write){};

    filesystem_t Find() const;
    filesystem_t Find(filesystem_t const& override) const;
    FileFinder& ClearSearchPath();
    size_t SearchPathSize() const;

    template <typename T>
    FileFinder& AppendSearchPath(T&&);

    template <typename T, typename... Args>
    FileFinder& AppendSearchPath(T&&, Args&&...);

    template <typename T>
    FileFinder& PrependSearchPath(T&& s);

    template <typename T, typename... Args>
    FileFinder& PrependSearchPath(T&& s, Args&&... paths);

   protected:
    bool FileMeetsRequirements(filesystem_t const& path) const;

    std::vector<filesystem_t> search_path;
    bool req_read;
    bool req_write;
};

// --- TEMPLATE DEFINITIONS BELOW --- //

template <typename T>
FileFinder& FileFinder::AppendSearchPath(T&& path)
{
    search_path.push_back(path);
    return *this;
}

template <typename T, typename... Args>
FileFinder& FileFinder::AppendSearchPath(T&& path, Args&&... paths)
{
    search_path.push_back(path);
    AppendSearchPath(std::forward<T>(paths)...);
    return *this;
}

template <typename T>
FileFinder& FileFinder::PrependSearchPath(T&& s)
{
    search_path.insert(search_path.begin(), s);
    return *this;
}

template <typename T, typename... Args>
FileFinder& FileFinder::PrependSearchPath(T&& s, Args&&... paths)
{
    search_path.insert(search_path.begin(), s);
    PrependSearchPath(std::forward<T>(paths)...);
    return *this;
}

}  // namespace bes

#endif
