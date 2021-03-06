#pragma once

#include "../data/symbol_shell.h"
#include "../syntax/expression.h"
#include "expression_node.h"

namespace bes::templating::node {

class ValueNode : public ExpressionNode
{
   public:
    using ExpressionNode::ExpressionNode;

    void render(std::ostringstream& ss, data::Context& ctx, data::TemplateStack& ts) const override
    {
        if (expr.right.symbol_type == syntax::Symbol::SymbolType::FUNCTION) {
            /// Call to macro
            MacroNode const* macro;
            try {
                macro = dynamic_cast<MacroNode const*>(ctx.getMacro(expr.left.value<std::string>()));
            } catch (std::exception& e) {
                throw TemplateException("Call to undefined macro: " + expr.left.value<std::string>() + "; " + e.what());
            }

            macro->menderMacro(ss, ctx, ts, expr.right.items);

        } else if (!expr.filters.empty()) {
            /// Filtered value
            std::ostringstream tmp;
            data::SymbolShell(expr.left, ctx).render(tmp);
            std::string filter_data = tmp.str();

            // Apply filters
            auto const* base = dynamic_cast<node::RootNode const*>(root);
            if (base->filters != nullptr) {
                for (auto const& filter_name : expr.filters) {
                    auto const& it = base->filters->find(filter_name);
                    if (it == base->filters->end()) {
                        throw TemplateException("Call to undefined filter: " + filter_name);
                    }
                    it->second(filter_data);
                }
            }

            ss << filter_data;
        } else {
            /// Raw value
            data::SymbolShell(expr.left, ctx).render(ss);
        }
    }
};

}  // namespace bes::templating::node
