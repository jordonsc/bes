#include <bes/templating.h>
#include <gtest/gtest.h>

#include <vector>

using s_vec = std::vector<std::string>;

TEST(TemplatingTextTest, StringSplit)
{
    using bes::templating::Text;
    auto out = Text::Split(" hello \"Mighty World\"     world", " ");
    EXPECT_EQ(4, out.size());
    EXPECT_EQ(out[0], "hello");
    EXPECT_EQ(out[1], "\"Mighty");
    EXPECT_EQ(out[2], "World\"");
    EXPECT_EQ(out[3], "world");
}

TEST(TemplatingTextTest, StringSplitArgs)
{
    using bes::templating::Text;

    {
        auto out = Text::SplitArgs(" hello \"Mighty World\"     world");
        EXPECT_EQ(3, out.size());
        EXPECT_EQ(out[0], "hello");
        EXPECT_EQ(out[1], "\"Mighty World\"");
        EXPECT_EQ(out[2], "world");
    }

    {
        auto out = Text::SplitArgs(" [hello, boys] \"Hello World\"");
        EXPECT_EQ(2, out.size());
        EXPECT_EQ(out[0], "[hello, boys]");
        EXPECT_EQ(out[1], "\"Hello World\"");
    }

    {
        auto out = Text::SplitArgs(" omg(\"hello boys\", hahaha) \"Hello World\"");
        EXPECT_EQ(3, out.size());
        EXPECT_EQ(out[0], "omg");
        EXPECT_EQ(out[1], "(\"hello boys\", hahaha)");
        EXPECT_EQ(out[2], "\"Hello World\"");
    }

    EXPECT_THROW(Text::SplitArgs(" [hello, boys \"Hello World\""), bes::templating::TemplateException);
    EXPECT_THROW(Text::SplitArgs(" [hello, boys] \"Hello World"), bes::templating::TemplateException);
}
