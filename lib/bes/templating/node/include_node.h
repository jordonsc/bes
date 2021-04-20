#pragma once

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

    void render(std::ostringstream& ss, bes::templating::data::Context& ctx, data::TemplateStack& ts) const override
    {
        ctx.increaseStack();

        bes::templating::RenderingInterface* eng = ts.engine();
        if (eng == nullptr) {
            throw TemplateException("Cannot render include '" + name + "': no engine provided in context");
        }

        // Will create a new TemplateStack for this sub-render
        ss << eng->render(name, ctx);

        ctx.decreaseStack();
    }
};

}  // namespace bes::templating::node
