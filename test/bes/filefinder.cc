#include "gtest/gtest.h"
#include "lib/bes/bes.h"

TEST(BesCoreTest, FileFinder)
{
    bes::FileFinder ff({"a", "b", "c"});
    EXPECT_EQ(3, ff.SearchPathSize());
    ff.AppendSearchPath("d", "e", "f");
    EXPECT_EQ(6, ff.SearchPathSize());
    ff.PrependSearchPath("h", "i", "j");
    EXPECT_EQ(9, ff.SearchPathSize());
    ff.ClearSearchPath();
    EXPECT_EQ(0, ff.SearchPathSize());
}
