#ifndef BES_TEMPLATING_NODE_MACRO_NODE_H
#define BES_TEMPLATING_NODE_MACRO_NODE_H

#include <memory>

#include "../data/context.h"
#include "../data/symbol_shell.h"
#include "expression_node.h"

namespace bes::templating::node {

/**
 * Macro nodes act as a function call, including its child nodes with context to the function arguments
 */
class MacroNode : public ExpressionNode
{
   public:
    using ExpressionNode::ExpressionNode;

    [[nodiscard]] size_t ArgCount() const
    {
        return expr.right.items.size();
    }

    [[nodiscard]] std::vector<std::any> const& GetArgs() const
    {
        return expr.right.items;
    }

    /**
     * Rendering the actual occurrence of {% macro ... %}.
     *
     * When we hit this, we just add the block to the TemplateStack for later referencing.
     */
    void Render(std::ostringstream& ss, data::Context& ctx, data::TemplateStack& ts) const override
    {
        ctx.AddMacro(expr.left.Value<std::string>(), dynamic_cast<Node const*>(this));
    }

    /**
     * A ValueNode has requested we render our actual content.
     */
    void RenderMacro(std::ostringstream& ss, data::Context& ctx, data::TemplateStack& ts,
                     std::vector<std::any> const& args) const
    {
        if (ArgCount() != args.size()) {
            throw TemplateException("Argument count mismatch for macro call: " + expr.raw);
        }

        ctx.IncreaseStack();

        // Add all function arguments as context items matching macro arguments (by position)
        size_t pos = 0;
        for (auto& arg : GetArgs()) {
            ctx.SetValue(
                std::any_cast<syntax::Symbol>(arg).Value<std::string>(),
                std::make_shared<bes::templating::data::SymbolShell>(std::any_cast<syntax::Symbol>(args[pos]), ctx));
            ++pos;
        }

        for (auto& node : child_nodes) {
            node->Render(ss, ctx, ts);
        }

        ctx.DecreaseStack();
    }
};

}  // namespace bes::templating::node

#endif
