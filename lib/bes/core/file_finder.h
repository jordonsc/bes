#pragma once

#include <bes/log.h>

#include <filesystem>
#include <utility>
#include <vector>

namespace bes {

using filesystem_t = std::string;

class FileFinder
{
   public:
    explicit inline FileFinder(bool require_read = true, bool require_write = false)
        : req_read(require_read), req_write(require_write)
    {}

    explicit inline FileFinder(std::vector<filesystem_t> search_paths, bool require_read = true,
                               bool require_write = false)
        : search_path(std::move(search_paths)), req_read(require_read), req_write(require_write){};

    /**
     * Find an exact file match.
     *
     * The search path should contain filenames.
     */
    [[nodiscard]] filesystem_t find() const;
    [[nodiscard]] filesystem_t find(filesystem_t const& override) const;

    /**
     * Find the given filename in the search path.
     *
     * The search path should contain path names.
     */
    [[nodiscard]] filesystem_t findInPath(filesystem_t const& filename) const;

    FileFinder& clearSearchPath();
    [[nodiscard]] size_t searchPathSize() const;
    [[nodiscard]] bool empty() const;

    template <typename T>
    FileFinder& AppendSearchPath(T&&);

    template <typename T, typename... Args>
    FileFinder& AppendSearchPath(T&&, Args&&...);

    template <typename T>
    FileFinder& PrependSearchPath(T&& s);

    template <typename T, typename... Args>
    FileFinder& PrependSearchPath(T&& s, Args&&... paths);

   protected:
    [[nodiscard]] bool fileMeetsRequirements(filesystem_t const& path) const;

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
