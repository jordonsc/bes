#ifndef BES_TEMPLATING_NODE_BLOCK_NODE_H
#define BES_TEMPLATING_NODE_BLOCK_NODE_H

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

    void Render(std::ostringstream& ss, data::Context& ctx, data::TemplateStack& ts) const override
    {
        ctx.IncreaseStack();

        auto const* child = ts.GetNextChildTemplate();

        if (child != nullptr && child->HasBlock(name)) {
            // Render to a temp context variable for 'super'
            std::ostringstream temp;
            for (auto& node : child_nodes) {
                node->Render(temp, ctx, ts);
            }

            ctx.SetValue("super", std::make_shared<data::StandardShell<std::string>>(temp.str()));

            ts.NextChild();
            child->RenderBlock(name, ss, ctx, ts);
            ts.PrevChild();
        } else {
            // Render directly to output
            for (auto& node : child_nodes) {
                node->Render(ss, ctx, ts);
            }
        }

        ctx.DecreaseStack();
    }
};

}  // namespace bes::templating::node

#endif
