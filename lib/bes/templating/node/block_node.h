#pragma once

#include "../data/std_shells.h"
#include "named_node.h"
#include "root_node.h"

namespace bes::templating::node {

/**
 * Block nodes do little special except contain a name. They are containers that allow child templates to extend them.
 */
class BlockNode : public NamedNode
{
   public:
    using NamedNode::NamedNode;

    void render(std::ostringstream& ss, data::Context& ctx, data::TemplateStack& ts) const override
    {
        ctx.increaseStack();

        auto const* child = ts.getNextChildTemplate();

        if (child != nullptr && child->hasBlock(name)) {
            // Render to a temp context variable for 'super'
            std::ostringstream temp;
            for (auto& node : child_nodes) {
                node->render(temp, ctx, ts);
            }

            ctx.setValue("super", std::make_shared<data::StandardShell<std::string>>(temp.str()));

            ts.nextChild();
            child->renderBlock(name, ss, ctx, ts);
            ts.prevChild();
        } else {
            // Render directly to output
            for (auto& node : child_nodes) {
                node->render(ss, ctx, ts);
            }
        }

        ctx.decreaseStack();
    }
};

}  // namespace bes::templating::node

