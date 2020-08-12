#ifndef BES_TEMPLATING_NODE_FOR_NODE_H
#define BES_TEMPLATING_NODE_FOR_NODE_H

#include <memory>

#include "../data/context.h"
#include "../data/shell_interface.h"
#include "../data/symbol_shell.h"
#include "expression_node.h"
#include "loop.h"

namespace bes::templating::node {

/**
 * For nodes contain child nodes that are run in a loop defined by the expression
 */
class ForNode : public ExpressionNode
{
   public:
    using ExpressionNode::ExpressionNode;

    void Render(std::ostringstream& ss, data::Context& ctx, data::TemplateStack& ts) const override
    {
        ctx.IncreaseStack();
        // Guarantees of symbol types is done during parsing
        auto var = expr.left.Value<std::string>();

        if (expr.right.symbol_type == syntax::Symbol::SymbolType::ARRAY) {
            // Loop over a literal array
            size_t pos = 0;
            for (auto& item : expr.right.items) {
                // Set context for the array item
                ctx.SetValue(var, std::make_shared<data::SymbolShell>(std::any_cast<syntax::Symbol>(item), ctx));

                // Set the loop context
                ctx.SetValue("loop",
                             std::make_shared<data::StandardShell<node::loop>>(loop(pos, expr.right.items.size())));

                // Process children
                for (auto& node : child_nodes) {
                    node->Render(ss, ctx, ts);
                }

                ++pos;
            }

        } else {
            // Create the array from the context map
            std::shared_ptr<data::ShellInterface> sh = GetRhsShell(ctx);

            size_t size = sh->Count();
            sh->Begin();
            for (size_t pos = 0; pos < size; ++pos) {
                // Set context for the object yield
                ctx.SetValue(var, sh->Yield());

                // Set the loop context
                ctx.SetValue("loop", std::make_shared<data::StandardShell<node::loop>>(loop(pos, size)));

                // Process children
                for (auto& node : child_nodes) {
                    node->Render(ss, ctx, ts);
                }
            }
        }

        ctx.DecreaseStack();
    }

   protected:
    /**
     * Recursive function to pull RHS items from the context
     */
    [[nodiscard]] std::shared_ptr<data::ShellInterface> ShellFromContext(
        std::shared_ptr<data::ShellInterface> const& item, size_t pos = 1) const
    {
        std::shared_ptr<data::ShellInterface> sub = item->ChildNode(std::any_cast<std::string>(expr.right.items[pos]));

        if (pos == expr.right.items.size() - 1) {
            return sub;
        } else {
            return ShellFromContext(sub, pos + 1);
        }
    }

    std::shared_ptr<data::ShellInterface> GetRhsShell(data::Context& ctx) const
    {
        std::shared_ptr<data::ShellInterface> item = ctx.GetValue(std::any_cast<std::string>(expr.right.items[0]));
        if (expr.right.items.size() > 1) {
            return ShellFromContext(item);
        } else {
            return item;
        }
    }
};

}  // namespace bes::templating::node

#endif
