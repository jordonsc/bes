#include <bes/dbal.h>
#include <gtest/gtest.h>

using namespace bes::dbal;

TEST(ContextTest, Context)
{
    Context c;

    EXPECT_FALSE(c.hasParameter("foo"));
    EXPECT_THROW(c.getParameter("foo"), OutOfRangeException);

    EXPECT_EQ(c.getOr("foo", "default"), "default");

    c.setParameter("foo", "bar");
    EXPECT_TRUE(c.hasParameter("foo"));
    EXPECT_EQ(c.getParameter("foo"), "bar");
}

TEST(ContextTest, ContextWithMap)
{
    std::unordered_map<std::string, std::string> map;
    map["foo"] = "bar";

    Context c(std::move(map));
    EXPECT_TRUE(c.hasParameter("foo"));
    EXPECT_FALSE(c.hasParameter("bar"));

    EXPECT_EQ(c.getParameter("foo"), "bar");
    EXPECT_THROW(c.getParameter("bar"), OutOfRangeException);
}
