#include "parser.h"

using namespace bes::templating;

Parser& Parser::Parse(node::RootNode& root, std::string const& content)
{
    size_t pos = 0;
    Parse(root, content, pos, &root);
    return *this;
}

/**
 * Template parser.
 *
 * Ideally, we'll remain as close to this document as the schema for this templating engine:
 * https://jinja.palletsprojects.com/en/2.11.x/templates/
 */
syntax::Expression Parser::Parse(node::Node& node, std::string const& content, size_t& position, node::RootNode* root)
{
    size_t max_parse = content.length() - Parser::MinSegmentSize;
    using bes::templating::syntax::Expression;

    // Trackers for trimming text nodes
    node::TextNode* prev = nullptr;
    bool trim_next = false;

    // Used for skipping past non-tag characters (eg a single '{')
    size_t advance_cursor = 0;

    while (position < content.length()) {
        size_t node_pos = content.find('{', position + advance_cursor);
        advance_cursor = 0;

        // The rest of the content is a TextNode
        if (node_pos == std::string::npos || node_pos > max_parse) {
            prev = new node::TextNode(std::string(content, position));
            if (trim_next) {
                prev->TrimFront();
            }
            node.AllocateNode(prev);
            break;
        }

        // Used to retrospectively add a TextNode
        size_t old_pos = position;

        // Use the Tag struct to parse the content of the tag; this struct will advance the position cursor
        syntax::Tag tag(content, node_pos, position);

        // False alarm, set the advance cursor and try again
        if (tag.tag_type == syntax::Tag::TagType::NONE) {
            position = old_pos;
            advance_cursor = node_pos - position + 1;
            continue;
        }

        // There is text before the next tag begins, add it as a TextNode
        if (node_pos > old_pos) {
            prev = new node::TextNode(std::string(content, old_pos, node_pos - old_pos));
            if (trim_next) {
                prev->TrimFront();
                trim_next = false;
            }
            node.AllocateNode(prev);
        }

        // Trim text nodes if we use the hyphen operator inside the tags, eg: {{- somevalue -}}
        if (tag.clear_left && prev != nullptr) {
            prev->TrimBack();
            prev = nullptr;
        }

        if (tag.clear_right) {
            trim_next = true;
        }

        // Finally, we can process the tag we found
        switch (tag.tag_type) {
            case syntax::Tag::TagType::COMMENT:
                // Comment tags we can just skip past and remove from the processed template
                break;
            case syntax::Tag::TagType::VALUE:
                // Value nodes are single, in-line, tags
                node.AllocateNode(new node::ValueNode(Expression(tag.raw), root));
                break;
            case syntax::Tag::TagType::CONTROL:
                // Control nodes are blocks with child-nodes, we expect an end-tag now
                {
                    Expression exp(tag.raw);
                    if (exp.end_tag) {
                        // If we're an end-tag off, return with the expression
                        return exp;
                    } else {
                        // Process this expression with a recursive control-block processor
                        ParseExpression(exp, node, content, position, root);
                    }
                }
                break;
            case syntax::Tag::TagType::NONE:
            default:
                throw TemplateException("Unknown templating tag");
        }
    }

    return Expression();
}

void Parser::ParseExpression(syntax::Expression const& exp, node::Node& node, std::string const& content,
                             size_t& position, node::RootNode* root)
{
    using bes::templating::syntax::Expression;

    switch (exp.clause) {
        case Expression::Clause::EXTENDS:
            // extends - the only clause NOT to have an end-tag, and we won't add this as a node
            {
                if (root->Extends()) {
                    throw TemplateException("Extends clause duplicated");
                } else if (root->NodeCount()) {
                    BES_LOG(WARNING) << "Template '" << root->Name() << "' extends clause is not the very first node";
                }

                root->SetExtends(exp.left.Value<std::string>());
            }
            break;
        case Expression::Clause::BLOCK:
            // block .. endblock
            {
                auto* sub_node = new node::BlockNode(exp.left.Value<std::string>(), root);
                Expression end_tag = Parse(*sub_node, content, position, root);

                if (end_tag.clause != Expression::Clause::ENDBLOCK) {
                    throw MissingEndTagException("Missing or incorrect end-tag for block node '" + exp.left.raw +
                                                 "', found: " + end_tag.raw);
                }

                // For block nodes, we'll add a shared pointer to the same node to both the parent node and the root
                // node's block list
                std::shared_ptr<node::BlockNode> ptr(sub_node);
                node.AddNode(ptr);
                root->AddBlock(ptr->Name(), ptr);
            }
            break;
        case Expression::Clause::FOR:
            // for .. endif
            {
                auto* sub_node = new node::ForNode(exp, root);
                Expression end_tag = Parse(*sub_node, content, position, root);

                if (end_tag.clause != Expression::Clause::ENDFOR) {
                    throw MissingEndTagException("Missing or incorrect end-tag for for-loop '" + exp.raw +
                                                 "', found: " + end_tag.raw);
                }

                node.AllocateNode(sub_node);
            }
            break;
        case Expression::Clause::IF:
        case Expression::Clause::ELIF:
        case Expression::Clause::ELSE:
            // if .. elif .. else .. endif
            {
                auto* sub_node = new node::IfNode(exp, root);
                node.AllocateNode(sub_node);

                Expression end_tag = Parse(*sub_node, content, position, root);

                switch (end_tag.clause) {
                    case Expression::Clause::ELIF:
                        if (exp.clause == Expression::Clause::ELSE) {
                            throw TemplateException("Cannot have an elif tag proceed an else tag");
                        }
                        ParseExpression(end_tag, node, content, position, root);
                        break;
                    case Expression::Clause::ELSE:
                        if (exp.clause == Expression::Clause::ELSE) {
                            throw TemplateException("Cannot have an else tag proceed an else tag");
                        }
                        ParseExpression(end_tag, node, content, position, root);
                        break;
                    case Expression::Clause::ENDIF:
                        break;
                    default:
                        throw MissingEndTagException("Missing or incorrect end-tag for if condition '" + exp.raw +
                                                     "', found: " + end_tag.raw);
                }
            }
            break;
        default:
            throw TemplateException("Unknown control tag: " + exp.raw);
    }
}
