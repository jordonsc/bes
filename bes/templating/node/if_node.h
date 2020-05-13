#ifndef BES_TEMPLATING_NODE_IF_NODE_H
#define BES_TEMPLATING_NODE_IF_NODE_H

#include "../data/context.h"
#include "../data/std_shells.h"
#include "../data/symbol_shell.h"
#include "expression_node.h"
#include "loop.h"

namespace bes::templating::node {

/**
 * If nodes control the if .. elif .. endif control flow.
 */
class IfNode : public ExpressionNode
{
   public:
    using ExpressionNode::ExpressionNode;

   protected:
    inline static std::string RenderSymbol(bes::templating::syntax::Symbol const& s, data::Context& context)
    {
        std::ostringstream temp;
        data::SymbolShell(s, context).Render(temp);
        return temp.str();
    }

    bool GetConditionState(data::Context& ctx) const
    {
        using bes::templating::syntax::Expression;

        bool condition_state;

        switch (expr.op) {
            case Expression::Operator::NONE:
                // Validate value is true
                condition_state = data::SymbolShell(expr.left, ctx).IsTrue();
                break;
            case Expression::Operator::IS_DEFINED:
                try {
                    // This will force the symbol to be resolved, the result of IsTrue() is meaningless
                    data::SymbolShell(expr.left, ctx).IsTrue();
                    condition_state = true;
                } catch (MissingContextException&) {
                    condition_state = false;
                }
                break;
            case Expression::Operator::IN:
                // TODO:
                throw TemplateException("Unimplemented: operator IN");
            case Expression::Operator::EQUALS:
                try {
                    condition_state = data::SymbolShell(expr.left, ctx) == data::SymbolShell(expr.right, ctx);
                } catch (ValueErrorException const&) {
                    condition_state = RenderSymbol(expr.left, ctx) == RenderSymbol(expr.right, ctx);
                }
                break;
            case Expression::Operator::NOT_EQUALS:
                try {
                    condition_state = data::SymbolShell(expr.left, ctx) != data::SymbolShell(expr.right, ctx);
                } catch (ValueErrorException const&) {
                    condition_state = RenderSymbol(expr.left, ctx) != RenderSymbol(expr.right, ctx);
                }
                break;
            case Expression::Operator::LT:
                condition_state = data::SymbolShell(expr.left, ctx) < data::SymbolShell(expr.right, ctx);
                break;
            case Expression::Operator::LTE:
                condition_state = data::SymbolShell(expr.left, ctx) <= data::SymbolShell(expr.right, ctx);
                break;
            case Expression::Operator::GT:
                condition_state = data::SymbolShell(expr.left, ctx) > data::SymbolShell(expr.right, ctx);
                break;
            case Expression::Operator::GTE:
                condition_state = data::SymbolShell(expr.left, ctx) >= data::SymbolShell(expr.right, ctx);
                break;

            default:
                throw TemplateException("Unsupported if clause: " + expr.raw);
        }

        if (expr.negated) {
            condition_state = !condition_state;
        }

        return condition_state;
    }

    void Render(std::ostringstream& ss, data::Context& ctx) const override
    {
        using CtrlType = bes::templating::syntax::Expression::Clause;

        // Check if a previous IF control block succeeded, whereby an ELSE block cannot execute
        if ((expr.clause == CtrlType::ELIF || expr.clause == CtrlType::ELSE) && (ctx.GetValue("_last_if")->IsTrue())) {
            // A previous IF/ELIF tag succeeded
            return;
        }

        // Check the result of the expression and store it for future ELIF/ELSE blocks
        if (expr.clause != bes::templating::syntax::Expression::Clause::ELSE) {
            bool condition_state = GetConditionState(ctx);
            ctx.SetValue("_last_if", std::make_shared<bes::templating::data::StandardShell<bool>>(condition_state));

            if (!condition_state) {
                return;
            }
        }

        // Process children
        ctx.IncreaseStack();
        for (auto& node : child_nodes) {
            node->Render(ss, ctx);
        }
        ctx.DecreaseStack();
    }
};  // namespace bes::templating::node

}  // namespace bes::templating::node

#endif
