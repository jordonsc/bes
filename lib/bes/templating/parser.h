#ifndef BES_TEMPLATING_PARSER_H
#define BES_TEMPLATING_PARSER_H

#include <bes/log.h>

#include <memory>

#include "exception.h"
#include "node/block_node.h"
#include "node/for_node.h"
#include "node/if_node.h"
#include "node/include_node.h"
#include "node/root_node.h"
#include "node/text_node.h"
#include "node/value_node.h"
#include "syntax/expression.h"
#include "syntax/tag.h"

namespace bes::templating {

class Parser
{
   public:
    Parser& Parse(node::RootNode& root, std::string const& content);

   protected:
    /// The smallest remaining chars before we accept that there isn't enough space to parse a tag: {{x}}
    constexpr static size_t const MinSegmentSize = 5;
    syntax::Expression ParseInner(node::Node&, std::string const&, size_t&, node::RootNode*);
    void ParseExpression(const syntax::Expression&, node::Node&, std::string const&, size_t&, node::RootNode*);
};

}  // namespace bes::templating

#endif
