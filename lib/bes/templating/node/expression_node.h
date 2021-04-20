#pragma once

#include <utility>

#include "../syntax/expression.h"
#include "node.h"

namespace bes::templating::node {

/**
 * Node contains an expression in its opening tag
 */
class ExpressionNode : public Node
{
   public:
    explicit ExpressionNode(bes::templating::syntax::Expression exp, Node const* const root = nullptr)
        : Node::Node(root), expr(std::move(exp))
    {}

   protected:
    bes::templating::syntax::Expression expr;
};

}  // namespace bes::templating::node
