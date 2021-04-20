#pragma once

#include <memory>
#include <string>

#include "exception.h"

namespace bes {

/**
 * C-style sprintf formatter for C++ strings.
 *
 * To be deprecated when we can use C++20.
 */
template <typename... Args>
std::string string_format(std::string const& format, Args... args)
{
    // Extra space for a trailing null character (C-strings)
    size_t size = snprintf(nullptr, 0, format.c_str(), args...) + 1;

    if (size <= 0) {
        throw bes::BesException(std::string("Error during string formatting"));
    }

    std::unique_ptr<char[]> buf(new char[size]);
    snprintf(buf.get(), size, format.c_str(), args...);

    // Remove the null at the end
    return std::string(buf.get(), buf.get() + size - 1);
}

}  // namespace bes
