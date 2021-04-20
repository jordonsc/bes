#pragma once

#define BES_TEMPLATING_INVALID_NOT "invalid use of 'not' keyword"
#define BES_TEMPLATING_DBL_NOT "double use of 'not' keyword"
#define BES_TEMPLATING_INCOMPLETE_EXPR "incomplete expression"

#include <sstream>
#include <string>
#include <vector>

#include "../exception.h"
#include "../text.h"
#include "symbol.h"

namespace bes::templating::syntax {

/**
 * Represents an expression inside template brackets {{ ... }} or {% ... %}
 */
struct Expression
{
    enum class Clause : char
    {
        NONE,

        // Start tags
        FOR,
        IF,
        VALUE,
        BLOCK,
        MACRO,
        FUNCTION,   // value type that calls a macro

        // End tags
        ENDFOR,
        ELIF,
        ELSE,
        ENDIF,
        ENDBLOCK,
        ENDMACRO,

        // Single tags with no end
        EXTENDS,
        INCLUDE,
    };

    enum class Operator : char
    {
        NONE,
        IN,
        IS,
        IS_DEFINED,

        EQUALS,
        NOT_EQUALS,
        LT,
        LTE,
        GT,
        GTE,
    };

    Expression() = default;

    /**
     * Creates an Expression from a plain text string.
     */
    explicit Expression(std::string const& str);

    Symbol left;
    Symbol right;
    bool negated = false;
    bool end_tag = false;
    Clause clause = Clause::NONE;
    Operator op = Operator::NONE;
    std::string raw;
    std::vector<std::string> filters;

   private:
    void validateComparisonOperator(bool neg, std::string const& str) const;
    void validateClause(bool neg, std::string const& str) const;
};

}  // namespace bes::templating::syntax
