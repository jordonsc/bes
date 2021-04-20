#include "root_node.h"

#include <bes/templating/node/macro_node.h>

using namespace bes::templating::node;

void RootNode::render(std::ostringstream& ss, data::Context& ctx, data::TemplateStack& ts) const
{
    for (auto& node : child_nodes) {
        node->render(ss, ctx, ts);
    }
}

bool RootNode::extends() const
{
    return is_extension;
}

std::string const& RootNode::extendsTemplate() const
{
    return extends_template;
}

void RootNode::setExtends(std::string const& name)
{
    extends_template = name;
    bes::templating::Text::trim(extends_template);

    if (extends_template.empty()) {
        throw TemplateException("Cannot extend a template with no name");
    }

    is_extension = true;
}

void RootNode::addBlock(std::string const& key, std::shared_ptr<Node> const& node)
{
    if (hasBlock(key)) {
        throw DuplicateBlockNodeException("Duplicate block node in template: " + key);
    }

    block_nodes[key] = node;
}

void RootNode::allocateBlock(std::string const& key, Node* node)
{
    // Important: we need to put `*node` under memory management before we throw any exceptions
    std::shared_ptr<Node> ptr(node);

    if (hasBlock(key)) {
        throw DuplicateBlockNodeException("Duplicate block node in template: " + key);
    }

    block_nodes[key] = ptr;
}

bool RootNode::hasBlock(std::string const& key) const
{
    return block_nodes.find(key) != block_nodes.end();
}

bool RootNode::renderBlock(std::string const& key, std::ostringstream& ss, bes::templating::data::Context& ctx,
                           data::TemplateStack& ts) const
{
    try {
        block_nodes.at(key)->render(ss, ctx, ts);
        return true;
    } catch (std::out_of_range const&) {
        return false;
    }
}
