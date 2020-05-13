#ifndef BES_TEMPLATNG_NODE_TEXT_NODE_H
#define BES_TEMPLATNG_NODE_TEXT_NODE_H

#include "node.h"

#include <utility>

namespace bes::templating::node {

/**
 * Contains a string of text only, should not contain any children.
 */
class TextNode : public Node
{
   public:
    explicit TextNode(std::string  content) : content(std::move(content)) {}

    inline void TrimFront()
    {
        bes::templating::Text::TrimFront(content);
    }

    inline void TrimBack()
    {
        bes::templating::Text::TrimBack(content);
    }

    void Render(std::ostringstream& str, data::Context& ctx) const override
    {
        str << content;
    }

   protected:
    std::string content;
};

}  // namespace bes::templating::node

#endif
