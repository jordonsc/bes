#include <vector>

#include "gtest/gtest.h"
#include "bes/templating.h"

using s_vec = std::vector<std::string>;

TEST(TemplatingTagTest, TagTest)
{
    using bes::templating::syntax::Tag;

    {
        std::string content = "blah blah x{{- value }}y blah blah blah";
        size_t parse_cursor = 0;
        size_t tag_cursor = content.find('{');

        Tag t = Tag(content, tag_cursor, parse_cursor);

        EXPECT_EQ(Tag::TagType::VALUE, t.tag_type);
        EXPECT_TRUE(t.clear_left);
        EXPECT_FALSE(t.clear_right);
        EXPECT_EQ(" value ", t.raw);
        EXPECT_EQ('y', content[parse_cursor]);
    }

    {
        std::string content = "blah blah a{{-value-}}b blah blah blah";
        size_t parse_cursor = 0;
        size_t tag_cursor = content.find('{');

        Tag t = Tag(content, tag_cursor, parse_cursor);

        EXPECT_EQ(Tag::TagType::VALUE, t.tag_type);
        EXPECT_TRUE(t.clear_left);
        EXPECT_TRUE(t.clear_right);
        EXPECT_EQ("value", t.raw);

        EXPECT_EQ('b', content[parse_cursor]);
    }

    {
        std::string content = "blah blah <{{-a-}}> blah blah blah";
        size_t parse_cursor = 0;
        size_t tag_cursor = content.find('{');

        Tag t = Tag(content, tag_cursor, parse_cursor);

        EXPECT_EQ(Tag::TagType::VALUE, t.tag_type);
        EXPECT_TRUE(t.clear_left);
        EXPECT_TRUE(t.clear_right);
        EXPECT_EQ("a", t.raw);
        EXPECT_EQ('>', content[parse_cursor]);
    }

    {
        std::string content = "blah blah <{{a}}> blah blah blah";
        size_t parse_cursor = 0;
        size_t tag_cursor = content.find('{');

        Tag t = Tag(content, tag_cursor, parse_cursor);

        EXPECT_EQ(Tag::TagType::VALUE, t.tag_type);
        EXPECT_FALSE(t.clear_left);
        EXPECT_FALSE(t.clear_right);
        EXPECT_EQ("a", t.raw);
        EXPECT_EQ('>', content[parse_cursor]);
    }

    {
        std::string content = "blah blah {{}} blah blah blah";
        size_t parse_cursor = 0;
        size_t tag_cursor = content.find('{');
        EXPECT_THROW(Tag(content, tag_cursor, parse_cursor), bes::templating::TemplateException);
    }

    {
        std::string content = "blah blah <{{-}}> blah blah blah";
        size_t parse_cursor = 0;
        size_t tag_cursor = content.find('{');
        EXPECT_THROW(Tag(content, tag_cursor, parse_cursor), bes::templating::TemplateException);
    }

    {
        std::string content = "blah blah <{{--}}> blah blah blah";
        size_t parse_cursor = 0;
        size_t tag_cursor = content.find('{');
        EXPECT_THROW(Tag(content, tag_cursor, parse_cursor), bes::templating::TemplateException);
    }
}
