#ifndef BES_TEMPLATING_NODE_ROOT_NODE_H
#define BES_TEMPLATING_NODE_ROOT_NODE_H

#include <unordered_map>

#include "../text.h"
#include "named_node.h"

namespace bes::templating::node {

/**
 * Root nodes are the base of a template. They contain a name and are the only node that tracks Block nodes specially.
 *
 * A root node may have an "extends" tag, indicating it is the child of another template.
 */
class RootNode : public NamedNode
{
    using NamedNode::NamedNode;

   public:
    void Render(std::ostringstream& ss, data::Context& ctx) const override;

    bool Extends() const;
    std::string const& ExtendsTemplate() const;
    void SetExtends(std::string const& name);

    void AddBlock(std::string const& key, std::shared_ptr<Node> const& node);
    void AllocateBlock(std::string const& key, Node* node);
    bool HasBlock(std::string const& key) const;
    bool RenderBlock(std::string const& key, std::ostringstream& ss, data::Context& ctx) const;

    std::unordered_map<std::string, Filter>* filters = nullptr;

   protected:
    std::string extends_template;
    bool is_extension = false;
    std::unordered_map<std::string, std::shared_ptr<Node>> block_nodes;
};

}  // namespace bes::templating::node

#endif
