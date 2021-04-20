#pragma once

#include <memory>
#include <sstream>
#include <vector>

#include "../data/context.h"
#include "../data/template_stack.h"
#include "../exception.h"

namespace bes::templating::node {

class Node
{
   public:
    explicit Node(Node const* const root = nullptr) : root(root) {}

    virtual void render(std::ostringstream&, data::Context&, data::TemplateStack&) const = 0;

    inline void addNode(std::shared_ptr<Node> const& node)
    {
        child_nodes.push_back(node);
    }

    inline void allocateNode(Node* node)
    {
        child_nodes.push_back(std::shared_ptr<Node>(node));
    }

    [[nodiscard]] inline size_t nodeCount() const
    {
        return child_nodes.size();
    }

   protected:
    std::vector<std::shared_ptr<Node>> child_nodes;
    Node const* const root;
};

}  // namespace bes::templating::node
