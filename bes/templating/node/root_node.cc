#include "root_node.h"

using namespace bes::templating::node;

void RootNode::Render(std::ostringstream& ss, data::Context& ctx) const
{
    for (auto& node : child_nodes) {
        node->Render(ss, ctx);
    }
}

bool RootNode::Extends() const
{
    return is_extension;
}

std::string const& RootNode::ExtendsTemplate() const
{
    return extends_template;
}

void RootNode::SetExtends(std::string const& name)
{
    extends_template = name;
    bes::templating::Text::Trim(extends_template);

    if (extends_template.empty()) {
        throw TemplateException("Cannot extend a template with no name");
    }

    is_extension = true;
}

void RootNode::AddBlock(std::string const& key, std::shared_ptr<Node> const& node)
{
    if (HasBlock(key)) {
        throw DuplicateBlockNodeException("Duplicate block node in template: " + key);
    }

    block_nodes[key] = node;
}

void RootNode::AllocateBlock(std::string const& key, Node* node)
{
    // Important: we need to put `*node` under memory management before we throw any exceptions
    std::shared_ptr<Node> ptr(node);

    if (HasBlock(key)) {
        throw DuplicateBlockNodeException("Duplicate block node in template: " + key);
    }

    block_nodes[key] = ptr;
}

bool RootNode::HasBlock(std::string const& key) const
{
    return block_nodes.find(key) != block_nodes.end();
}

bool RootNode::RenderBlock(std::string const& key, std::ostringstream& ss, bes::templating::data::Context& ctx) const
{
    try {
        block_nodes.at(key)->Render(ss, ctx);
        return true;
    } catch (std::out_of_range const&) {
        return false;
    }
}

void RootNode::SetChildTemplate(RootNode* child)
{
    child_template = child;
}

bool RootNode::HasChild() const
{
    return child_template != nullptr;
}

RootNode* RootNode::Child() const
{
    return child_template;
}
