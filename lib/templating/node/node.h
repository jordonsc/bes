#ifndef BES_TEMPLATING_BLOCKS_BLOCK_H
#define BES_TEMPLATING_BLOCKS_BLOCK_H

#include <memory>
#include <sstream>
#include <vector>

#include "../data/context.h"
#include "../exception.h"

namespace bes::templating::node {

class Node
{
   public:
    Node(Node const* const root = nullptr) : root(root) {}

    virtual void Render(std::ostringstream&, data::Context&) const = 0;

    inline void AddNode(std::shared_ptr<Node> const& node)
    {
        child_nodes.push_back(node);
    }

    inline void AllocateNode(Node* node)
    {
        child_nodes.push_back(std::shared_ptr<Node>(node));
    }

    inline size_t NodeCount() const
    {
        return child_nodes.size();
    }

   protected:
    std::vector<std::shared_ptr<Node>> child_nodes;
    Node const* const root;
};

}  // namespace bes::templating::node

#endif
