#pragma once

#include <any>
#include <cmath>
#include <string>
#include <vector>

#include "../exception.h"
#include "../text.h"

namespace bes::templating::syntax {

struct Symbol
{
    enum class SymbolType : char
    {
        NONE,
        VARIABLE,
        LITERAL,
        ARRAY,
        FUNCTION,
    };

    enum class DataType : char
    {
        NONE,
        VARIABLE,
        STRING,
        CHAR,
        INT,
        FLOAT,
        BOOL,
    };

    Symbol() = default;
    explicit Symbol(std::string const&);

    template <class T>
    T value(size_t pos = 0) const;

    std::vector<std::any> items;
    SymbolType symbol_type = SymbolType::NONE;
    DataType data_type = DataType::NONE;
    std::string raw;

    static bool isValidVariableName(std::string const& v);
};

template <class T>
inline T Symbol::value(size_t pos) const
{
    if (items.size() < (pos + 1)) {
        throw IndexErrorException("Symbol value index out of bounds: " + std::to_string(pos));
    }

    return std::any_cast<T>(items[pos]);
}

}  // namespace bes::templating::syntax
