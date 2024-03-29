#pragma once

#include <memory>

#include "../syntax/symbol.h"
#include "context.h"
#include "shell_interface.h"
#include "std_shells.h"

namespace bes::templating::data {

class SymbolShell : public ShellInterface
{
   public:
    SymbolShell(syntax::Symbol s, Context& ctx) : symbol(s), context(ctx) {}

    void render(std::ostringstream& str) const override
    {
        GetItemShell()->render(str);
    }

    std::shared_ptr<ShellInterface> childNode(std::string const& key) const override
    {
        // Considerations for indexing an array-symbol, useful if a {% set x = [..] %} tag existed
        if (symbol.symbol_type == syntax::Symbol::SymbolType::ARRAY && key.length() > 6) {
            // Allow array indexing operations
            if (key.substr(0, 5) == "item[" && key.substr(key.length() - 6) == "]") {
                int index = ::atoi(key.substr(5, key.length() - 1).c_str());
                if (index < 0 || index >= (int)symbol.items.size()) {
                    throw IndexErrorException("Array index out of bounds: index " + std::to_string(index) + " in " +
                                              symbol.raw);
                }

                return std::make_shared<SymbolShell>(symbol.value<syntax::Symbol>(index), context);
            }
        }

        throw IndexErrorException("Requested indexing of Symbol: " + key);
    }

    bool isTrue() const override
    {
        return GetItemShell()->isTrue();
    }

    long asInt() const override
    {
        return GetItemShell()->asInt();
    }

    double asFloat() const override
    {
        return GetItemShell()->asFloat();
    }

   private:
    /**
     * Recursive function to pull child items from the context
     */
    std::shared_ptr<ShellInterface> ShellFromContext(std::shared_ptr<ShellInterface> const& item, size_t pos = 1) const
    {
        std::shared_ptr<ShellInterface> sub = item->childNode(std::any_cast<std::string>(symbol.items[pos]));

        if (pos == symbol.items.size() - 1) {
            return sub;
        } else {
            return ShellFromContext(sub, pos + 1);
        }
    }

   protected:
    bes::templating::syntax::Symbol symbol;
    Context& context;

    /**
     * Get a shell for the item this symbol represents
     */
    std::shared_ptr<ShellInterface> GetItemShell() const
    {
        using bes::templating::syntax::Symbol;

        if (symbol.symbol_type == Symbol::SymbolType::LITERAL) {
            try {
                switch (symbol.data_type) {
                    case Symbol::DataType::STRING:
                        return std::make_shared<StandardShell<std::string>>(symbol.value<std::string>());
                    case Symbol::DataType::CHAR:
                        return std::make_shared<StandardShell<char>>(symbol.value<char>());
                    case Symbol::DataType::INT:
                        return std::make_shared<StandardShell<int>>(symbol.value<int>());
                    case Symbol::DataType::FLOAT:
                        return std::make_shared<StandardShell<float>>(symbol.value<float>());
                    case Symbol::DataType::BOOL:
                        return std::make_shared<StandardShell<bool>>(symbol.value<bool>());
                    default:
                        throw TemplateException("Cannot render symbol: " + symbol.raw);
                }
            } catch (std::exception& e) {
                throw TemplateException("Error rendering literal: " + symbol.raw + ": " + e.what());
            }
        } else if (symbol.symbol_type == Symbol::SymbolType::VARIABLE) {
            std::shared_ptr<ShellInterface> const& item = context.getValue(std::any_cast<std::string>(symbol.items[0]));
            if (symbol.items.size() > 1) {
                return ShellFromContext(item);
            } else {
                return item;
            }
        } else {
            throw TemplateException("Symbol rendering of given type is not supported: " + symbol.raw);
        }
    }
};

}  // namespace bes::templating::data
