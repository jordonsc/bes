#pragma once

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

    [[nodiscard]] size_t argCount() const
    {
        return expr.right.items.size();
    }

    [[nodiscard]] std::vector<std::any> const& getArgs() const
    {
        return expr.right.items;
    }

    /**
     * Rendering the actual occurrence of {% macro ... %}.
     *
     * When we hit this, we just add the block to the TemplateStack for later referencing.
     */
    void render(std::ostringstream& ss, data::Context& ctx, data::TemplateStack& ts) const override
    {
        ctx.addMacro(expr.left.value<std::string>(), dynamic_cast<Node const*>(this));
    }

    /**
     * A ValueNode has requested we render our actual content.
     */
    void menderMacro(std::ostringstream& ss, data::Context& ctx, data::TemplateStack& ts,
                     std::vector<std::any> const& args) const
    {
        if (argCount() != args.size()) {
            throw TemplateException("Argument count mismatch for macro call: " + expr.raw);
        }

        ctx.increaseStack();

        // Add all function arguments as context items matching macro arguments (by position)
        size_t pos = 0;
        for (auto& arg : getArgs()) {
            ctx.setValue(
                std::any_cast<syntax::Symbol>(arg).value<std::string>(),
                std::make_shared<bes::templating::data::SymbolShell>(std::any_cast<syntax::Symbol>(args[pos]), ctx));
            ++pos;
        }

        for (auto& node : child_nodes) {
            node->render(ss, ctx, ts);
        }

        ctx.decreaseStack();
    }
};

}  // namespace bes::templating::node
