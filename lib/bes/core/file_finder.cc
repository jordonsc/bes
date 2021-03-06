#include "file_finder.h"

#include <bes/log.h>

#include <experimental/filesystem>
#include <fstream>
#include <stdexcept>

#include "exception.h"

using namespace bes;
using fs_path = std::experimental::filesystem::path;

filesystem_t FileFinder::find() const
{
    for (auto const& path : search_path) {
        BES_LOG(TRACE) << "Scan: " << path;
        if (fileMeetsRequirements(path)) {
            BES_LOG(TRACE) << "Match: " << path;
            return path;
        }
    }

    throw FileNotFoundException("Cannot locate file", search_path.front());
}

filesystem_t FileFinder::find(filesystem_t const& override) const
{
    BES_LOG(TRACE) << "Scan: " << override;
    if (fileMeetsRequirements(override)) {
        BES_LOG(TRACE) << "Match: " << override;
        return override;
    } else {
        throw FileNotFoundException("Unable to open file", override);
    }
}

filesystem_t FileFinder::findInPath(filesystem_t const& filename) const
{
    for (auto const& path : search_path) {
        std::string fn = path;
        fn += fs_path::preferred_separator;
        fn += filename;

        BES_LOG(TRACE) << "Scan: " << fn;
        if (fileMeetsRequirements(fn)) {
            BES_LOG(TRACE) << "Match: " << fn;
            return fn;
        }
    }

    throw FileNotFoundException("Cannot locate file", filename);
}

FileFinder& FileFinder::clearSearchPath()
{
    search_path.clear();
    return *this;
}

bool FileFinder::fileMeetsRequirements(filesystem_t const& path) const
{
    std::fstream s;

    if (req_read && req_write) {
        // Open for read & write
        s.open(path, std::ios_base::in | std::ios_base::out);
    } else if (req_read) {
        // Open for read only
        s.open(path, std::ios_base::in);
    } else if (req_write) {
        // Open for write only
        s.open(path, std::ios_base::out);
    } else {
        // We don't actually need the file to be accessible
        throw std::runtime_error("No validation on file check: " + path);
    }

    return s.is_open();
}

size_t FileFinder::searchPathSize() const
{
    return search_path.size();
}

bool FileFinder::empty() const
{
    return search_path.empty();
}
