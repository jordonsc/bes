#ifndef BES_TEMPLATING_SYNTAX_TAG_H
#define BES_TEMPLATING_SYNTAX_TAG_H

#include <string>

#include "../exception.h"

namespace bes::templating::syntax {

struct Tag
{
    enum class TagType : char
    {
        NONE,
        VALUE,
        CONTROL,
        COMMENT,
    };

    Tag(std::string const& content, size_t& tag_pos, size_t& parse_cursor);

    TagType tag_type = TagType::NONE;
    bool clear_left = false;
    bool clear_right = false;
    std::string raw;
};

}  // namespace bes::templating::syntax

#endif
