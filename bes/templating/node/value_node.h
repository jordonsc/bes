#ifndef BES_TEMPLATING_NODE_VALUE_NODE_H
#define BES_TEMPLATING_NODE_VALUE_NODE_H

#include "../data/symbol_shell.h"
#include "../syntax/expression.h"
#include "expression_node.h"

namespace bes::templating::node {

class ValueNode : public ExpressionNode
{
   public:
    using ExpressionNode::ExpressionNode;

    void Render(std::ostringstream& ss, data::Context& ctx, data::TemplateStack& ts) const override
    {
        if (!expr.filters.empty()) {
            std::ostringstream tmp;
            data::SymbolShell(expr.left, ctx).Render(tmp);
            std::string filter_data = tmp.str();

            // Apply filters
            auto const* base = dynamic_cast<node::RootNode const*>(root);
            if (base->filters != nullptr) {
                for (auto const& filter_name : expr.filters) {
                    auto const& it = base->filters->find(filter_name);
                    if (it == base->filters->end()) {
                        throw TemplateException("Reference to undefined filter: " + filter_name);
                    }
                    it->second(filter_data);
                }
            }

            ss << filter_data;
        } else {
            data::SymbolShell(expr.left, ctx).Render(ss);
        }
    }
};

}  // namespace bes::templating::node

#endif
