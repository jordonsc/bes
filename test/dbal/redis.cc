#include <bes/dbal.redis.h>
#include <gtest/gtest.h>

using namespace bes::dbal;
using namespace bes::dbal::kv;

static Redis createDatabase()
{
    Context c;
    c.setParameter(redis::CFG_HOSTNAME, "localhost");

    return Redis(std::move(c));
}

TEST(RedisTest, Connection)
{
    Context c;
    c.setParameter(redis::CFG_HOSTNAME, "localhost");

    ASSERT_NO_THROW({ auto throwaway = Redis(c); });

    auto db = Redis(std::move(c));
    ASSERT_TRUE(db.isConnected());
}

TEST(RedisTest, StringData)
{
    auto db = createDatabase();

    // Validate data exists in DB -
    EXPECT_TRUE(db.apply("key 0", "zero").ok());
    EXPECT_EQ(db.retrieve("key 0").asString(), "zero");

    // Truncate to clear any test data
    EXPECT_TRUE(db.truncate().ok());

    // Validate we've lost our data
    EXPECT_THROW(db.retrieve("key 0").wait(), DoesNotExistException);

    // Test various string scenarios
    db.apply("key1", "value1").wait();
    db.apply("key 2", "value 2").wait();
    db.apply("key 3", "value 3").wait();

    // Reordered - should make no difference
    EXPECT_EQ(db.retrieve("key 3").asString(), "value 3");
    EXPECT_EQ(db.retrieve("key1").asString(), "value1");
    EXPECT_EQ(db.retrieve("key 2").asString(), "value 2");

    // Blank a value to ensure it doesn't trigger a DoesNotExistException
    EXPECT_TRUE(db.apply("key1", "").ok());

    auto f = db.retrieve("key1");
    EXPECT_NO_THROW(f.wait());
    EXPECT_EQ(f.asString(), "");
}

TEST(RedisTest, IntegerData)
{
    auto db = createDatabase();

    db.apply("num", Int64(10)).wait();
    EXPECT_EQ(db.retrieve("num").asInt(), 10);

    db.offset("num", Int64(3)).wait();
    EXPECT_EQ(db.retrieve("num").asInt(), 13);

    db.offset("num", Int64(-5)).wait();
    EXPECT_EQ(db.retrieve("num").asInt(), 8);

    EXPECT_EQ(db.retrieve("num").asString(), "8");

    db.apply("notanum", "hola");
    db.offset("notanum", Int64(3));  // nothing will happen
    EXPECT_THROW(db.retrieve("notanum").asInt(), BadDataType);
}
