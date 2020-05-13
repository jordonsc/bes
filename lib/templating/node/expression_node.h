#ifndef BES_TEMPLATING_NODE_EXPRESSION_NODE_H
#define BES_TEMPLATING_NODE_EXPRESSION_NODE_H

#include "../syntax/expression.h"
#include "lib/log/log.h"
#include "node.h"

namespace bes::templating::node {

/**
 * Node contains an expression in its opening tag
 */
class ExpressionNode : public Node
{
   public:
    ExpressionNode(bes::templating::syntax::Expression const& exp, Node const* const root = nullptr)
        : Node::Node(root), expr(exp)
    {}

    ExpressionNode(bes::templating::syntax::Expression&& exp, Node const* const root = nullptr)
        : Node::Node(root), expr(exp)
    {}

   protected:
    bes::templating::syntax::Expression expr;
};

}  // namespace bes::templating::node

#endif
