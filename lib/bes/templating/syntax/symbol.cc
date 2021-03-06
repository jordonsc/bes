#include "symbol.h"

using namespace bes::templating::syntax;

bes::templating::syntax::Symbol::Symbol(std::string const& str)
{
    using bes::templating::Text;

    if (!str.length()) {
        throw MalformedSymbolException(str, "Symbol cannot be blank");
    }

    raw = str;
    char const first = str[0];
    char const last = str[str.length() - 1];

    if (first == '"') {
        // String literal
        if (last != '"') {
            throw MalformedSymbolException(str, "unterminated string");
        }

        symbol_type = SymbolType::LITERAL;
        data_type = DataType::STRING;
        items.emplace_back(std::string(str, 1, str.length() - 2));

    } else if (first == '\'') {
        // Char literal
        if (str.length() != 3 || last != '\'') {
            throw MalformedSymbolException(str, "Invalid character symbol");
        }

        symbol_type = SymbolType::LITERAL;
        data_type = DataType::CHAR;
        items.emplace_back(str[1]);

    } else if (first == '[') {
        // Array
        symbol_type = SymbolType::ARRAY;
        if (last != ']') {
            throw MalformedSymbolException(str, "unterminated array");
        }

        auto parts = Text::split(std::string(str, 1, str.length() - 2), ",");
        for (auto& part : parts) {
            Text::trim(part);
            items.emplace_back(Symbol(part));
        }
    } else if (first == '(') {
        // Array
        symbol_type = SymbolType::FUNCTION;
        if (last != ')') {
            throw MalformedSymbolException(str, "unterminated function syntax");
        }

        auto parts = Text::split(std::string(str, 1, str.length() - 2), ",");
        for (auto& part : parts) {
            Text::trim(part);
            items.emplace_back(Symbol(part));
        }
    } else if (first >= '0' && first <= '9') {
        // Numeric literal
        float v = 0;
        int dec = 0;
        symbol_type = SymbolType::LITERAL;

        for (char c : str) {
            if (c == '.') {
                if (dec) {
                    throw MalformedSymbolException(str, "invalid number sequence");
                } else {
                    ++dec;
                }
                continue;
            } else if (c < '0' || c > '9') {
                throw MalformedSymbolException(str, "not a numeric value");
            }

            if (dec) {
                v += float((c - '0') / std::pow(10, dec));
                ++dec;
            } else {
                v *= 10;
                v += float(c - '0');
            }
        }

        if (dec) {
            data_type = DataType::FLOAT;
            items.emplace_back(v);
        } else {
            data_type = DataType::INT;
            items.emplace_back(int(v));
        }

    } else if (str == "true") {
        // Boolean true
        symbol_type = SymbolType::LITERAL;
        data_type = DataType::BOOL;
        items.emplace_back(true);

    } else if (str == "false") {
        // Boolean false
        symbol_type = SymbolType::LITERAL;
        data_type = DataType::BOOL;
        items.emplace_back(false);

    } else {
        // Variable
        symbol_type = SymbolType::VARIABLE;
        data_type = DataType::VARIABLE;

        auto parts = Text::split(str, ".");
        for (auto& part : parts) {
            if (!isValidVariableName(part)) {
                throw MalformedSymbolException(str, "Invalid variable name");
            }
            items.emplace_back(part);
        }
    }
}

bool Symbol::isValidVariableName(std::string const& v)
{
    if (!v.length()) {
        return false;
    }

    bool is_first = true;
    for (auto& c : v) {
        if ((c < 'a' || c > 'z') && (c < 'A' || c > 'Z') && (c != '_')) {
            if (is_first) {
                return false;
            } else {
                if (c < '0' || c > '9') {
                    return false;
                }
            }
        }
        is_first = false;
    }

    return true;
}
