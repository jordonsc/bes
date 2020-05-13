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
        items.push_back(std::string(str, 1, str.length() - 2));

    } else if (first == '\'') {
        // Char literal
        if (str.length() != 3 || last != '\'') {
            throw MalformedSymbolException(str, "Invalid character symbol");
        }

        symbol_type = SymbolType::LITERAL;
        data_type = DataType::CHAR;
        items.push_back(str[1]);

    } else if (first == '[') {
        // Array
        symbol_type = SymbolType::ARRAY;
        if (last != ']') {
            throw MalformedSymbolException(str, "unterminated array");
        }

        auto parts = Text::Split(std::string(str, 1, str.length() - 2), ",");
        for (auto& part : parts) {
            Text::Trim(part);
            items.push_back(Symbol(part));
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
                v += (c - '0') / std::pow(10, dec);
                ++dec;
            } else {
                v *= 10;
                v += (c - '0');
            }
        }

        if (dec) {
            data_type = DataType::FLOAT;
            items.push_back(v);
        } else {
            data_type = DataType::INT;
            items.push_back(int(v));
        }

    } else if (str == "true") {
        // Boolean true
        symbol_type = SymbolType::LITERAL;
        data_type = DataType::BOOL;
        items.push_back(true);

    } else if (str == "false") {
        // Boolean false
        symbol_type = SymbolType::LITERAL;
        data_type = DataType::BOOL;
        items.push_back(false);

    } else {
        // Variable
        symbol_type = SymbolType::VARIABLE;
        data_type = DataType::VARIABLE;

        auto parts = Text::Split(str, ".");
        for (auto& part : parts) {
            if (!IsValidVariableName(part)) {
                throw MalformedSymbolException(str, "Invalid variable name");
            }
            items.push_back(part);
        }
    }
}

bool Symbol::IsValidVariableName(std::string const& v)
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
