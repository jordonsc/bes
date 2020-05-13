#ifndef BES_TEMPLATING_NODE_NAMED_NODE_H
#define BES_TEMPLATING_NODE_NAMED_NODE_H

#include "node.h"

namespace bes::templating::node {

/**
 * Abstract base class for a node containing a name.
 */
class NamedNode : public Node
{
   public:
    NamedNode(std::string const& name, Node const* const root = nullptr) : Node::Node(root), name(name) {}

    virtual inline std::string const& Name() const
    {
        return name;
    }

   protected:
    std::string const name;
};

}  // namespace bes::templating::node

#endif
