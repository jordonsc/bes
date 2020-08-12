#ifndef BES_TEMPLATING_NODE_INCLUDE_NODE_H
#define BES_TEMPLATING_NODE_INCLUDE_NODE_H

#include "../data/std_shells.h"
#include "../rendering_interface.h"
#include "named_node.h"

namespace bes::templating::node {

/**
 * Includes another template in its place.
 */
class IncludeNode : public NamedNode
{
   public:
    using NamedNode::NamedNode;

    void Render(std::ostringstream& ss, bes::templating::data::Context& ctx, data::TemplateStack& ts) const override
    {
        ctx.IncreaseStack();

        bes::templating::RenderingInterface* eng = ts.Engine();
        if (eng == nullptr) {
            throw TemplateException("Cannot render include '" + name + "': no engine provided in context");
        }

        // Will create a new TemplateStack for this sub-render
        ss << eng->Render(name, ctx);

        ctx.DecreaseStack();
    }
};

}  // namespace bes::templating::node

#endif
