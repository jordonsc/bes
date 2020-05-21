#include "tag.h"

using namespace bes::templating::syntax;

Tag::Tag(std::string const& content, size_t& tag_pos, size_t& parse_cursor)
{
    if (content.length() < tag_pos + 4) {
        parse_cursor = tag_pos;
        return;
    }

    size_t end_pos = 0;
    if (content[tag_pos + 1] == '{') {
        // Value tag, has no end tag
        end_pos = content.find("}}", tag_pos + 2);
        if (end_pos != std::string::npos) {
            tag_type = TagType::VALUE;
        } else {
            throw MissingEndTagException("Value tag {{ ... }} at position " + std::to_string(tag_pos) +
                                         " missing end tag");
        }
    } else if (content[tag_pos + 1] == '%') {
        // Control tag, requires a matching end tag
        end_pos = content.find("%}", tag_pos + 2);
        if (end_pos != std::string::npos) {
            tag_type = TagType::CONTROL;
        } else {
            throw MissingEndTagException("Control tag {% ... %} at position " + std::to_string(tag_pos) +
                                         " missing end tag");
        }
    } else if (content[tag_pos + 1] == '#') {
        // Comment tag, has no end tag
        end_pos = content.find("#}", tag_pos + 2);
        if (end_pos != std::string::npos) {
            tag_type = TagType::COMMENT;
        } else {
            throw MissingEndTagException("Comment tag {# ... #} at position " + std::to_string(tag_pos) +
                                         " missing end tag");
        }
    } else {
        parse_cursor = tag_pos;
        return;
    }

    parse_cursor = end_pos + 2;
    clear_left = content[tag_pos + 2] == '-';
    clear_right = content[end_pos - 1] == '-';
    size_t start_pos = tag_pos + (clear_left ? 3 : 2);

    if ((end_pos - tag_pos) < static_cast<size_t>(3 + (clear_left ? 1 : 0) + (clear_right ? 1 : 0))) {
        throw TemplateException("Template tags at position " + std::to_string(tag_pos) + " are too short");
    }

    raw = std::string(content, start_pos, end_pos - (clear_right ? start_pos + 1 : start_pos));
}
