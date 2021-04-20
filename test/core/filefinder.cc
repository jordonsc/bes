#include <bes/core.h>
#include <gtest/gtest.h>

TEST(BesCoreTest, FileFinder)
{
    bes::FileFinder ff({"a", "b", "c"});
    EXPECT_EQ(3, ff.searchPathSize());
    ff.AppendSearchPath("d", "e", "f");
    EXPECT_EQ(6, ff.searchPathSize());
    ff.PrependSearchPath("h", "i", "j");
    EXPECT_EQ(9, ff.searchPathSize());
    ff.clearSearchPath();
    EXPECT_EQ(0, ff.searchPathSize());
}
