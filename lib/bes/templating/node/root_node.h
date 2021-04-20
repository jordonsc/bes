#pragma once

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
    void render(std::ostringstream& ss, data::Context& ctx, data::TemplateStack& ts) const override;

    bool extends() const;
    std::string const& extendsTemplate() const;
    void setExtends(std::string const& name);

    // Block nodes
    void addBlock(std::string const& key, std::shared_ptr<Node> const& node);
    void allocateBlock(std::string const& key, Node* node);
    bool hasBlock(std::string const& key) const;
    bool renderBlock(std::string const& key, std::ostringstream& ss, data::Context& ctx, data::TemplateStack& ts) const;

    // Filters
    std::unordered_map<std::string, Filter>* filters = nullptr;

   protected:
    std::string extends_template;
    bool is_extension = false;
    std::unordered_map<std::string, std::shared_ptr<Node>> block_nodes;
};

}  // namespace bes::templating::node
