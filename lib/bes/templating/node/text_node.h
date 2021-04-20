#pragma once

#include <utility>

#include "node.h"

namespace bes::templating::node {

/**
 * Contains a string of text only, should not contain any children.
 */
class TextNode : public Node
{
   public:
    explicit TextNode(std::string content) : content(std::move(content)) {}

    inline void trimFront()
    {
        bes::templating::Text::trimFront(content);
    }

    inline void trimBack()
    {
        bes::templating::Text::trimBack(content);
    }

    void render(std::ostringstream& str, data::Context& ctx, data::TemplateStack& ts) const override
    {
        str << content;
    }

   protected:
    std::string content;
};

}  // namespace bes::templating::node
