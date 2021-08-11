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

    // Test the -not-exists functions
    EXPECT_TRUE(db.applyNx("key 2", "value nx").ok());
    EXPECT_EQ(db.retrieve("key 2").asString(), "value 2");

    // Test that on a value that doesn't exist
    EXPECT_TRUE(db.apply("key 4", "value nx").ok());
    EXPECT_EQ(db.retrieve("key 4").asString(), "value nx");
}

TEST(RedisTest, IntegerData)
{
    auto db = createDatabase();

    db.apply("num", Int64(10)).wait();
    EXPECT_EQ(db.retrieve("num").asInt(), 10);

    db.applyNx("num", Int64(12)).wait();
    EXPECT_EQ(db.retrieve("num").asInt(), 10);

    db.applyNx("num-nx", Int64(12)).wait();
    EXPECT_EQ(db.retrieve("num-nx").asInt(), 12);

    db.offset("num", Int64(3)).wait();
    EXPECT_EQ(db.retrieve("num").asInt(), 13);

    db.offset("num", Int64(-5)).wait();
    EXPECT_EQ(db.retrieve("num").asInt(), 8);

    EXPECT_EQ(db.retrieve("num").asString(), "8");

    db.apply("notanum", "hola");
    db.offset("notanum", Int64(3));  // nothing will happen
    EXPECT_THROW(db.retrieve("notanum").asInt(), BadDataType);
}

TEST(RedisTest, FloatData)
{
    auto db = createDatabase();

    db.apply("float", Float64(10.1)).wait();
    EXPECT_EQ(db.retrieve("float").asFloat(), 10.1);

    db.applyNx("float", Float64(12.1)).wait();
    EXPECT_EQ(db.retrieve("float").asFloat(), 10.1);

    db.applyNx("float-nx", Float64(12.5)).wait();
    EXPECT_EQ(db.retrieve("float-nx").asFloat(), 12.5);

    db.offset("float", Float64(3.3)).wait();
    EXPECT_EQ(db.retrieve("float").asFloat(), 13.4);

    db.offset("float", Float64(-5.2)).wait();
    EXPECT_EQ(db.retrieve("float").asFloat(), 8.2);

    EXPECT_EQ(db.retrieve("float").asString(), "8.2");

    db.apply("notanum", "hola");
    db.offset("notanum", Float64(3.1));  // nothing will happen
    EXPECT_THROW(db.retrieve("notanum").asFloat(), BadDataType);
}

TEST(RedisTest, Transactions)
{
    auto db = createDatabase();
    db.truncate().wait();
    db.apply("trans_test_a", "foo").wait();

    // Test abandoning the transaction
    EXPECT_TRUE(db.beginTransaction().ok());
    db.apply("trans_test", "some data").wait();
    db.apply("trans_test_a", "bar").wait();
    EXPECT_TRUE(db.discardTransaction().ok());

    EXPECT_EQ(db.retrieve("trans_test_a").asString(), "foo");
    EXPECT_THROW(db.retrieve("trans_test").wait(), DoesNotExistException);

    // Commit now, record should exist
    db.beginTransaction();
    db.apply("trans_test", "some data");
    db.apply("trans_test_a", "bar");
    db.commitTransaction().wait();

    EXPECT_EQ(db.retrieve("trans_test").asString(), "some data");
    EXPECT_EQ(db.retrieve("trans_test_a").asString(), "bar");

    // Apply and retrieve in the same transaction
    db.beginTransaction();
    auto a1 = db.apply("trans_test", "new data");
    auto a2 = db.apply("trans_test_a", "hello world");
    auto b = db.retrieve("trans_test");
    auto ba = db.retrieve("trans_test_a");
    db.commitTransaction();

    EXPECT_TRUE(a1.ok());
    EXPECT_TRUE(a2.ok());
    EXPECT_NE(b.asString(), "new data");    // Redis will return "QUEUED" for uncommitted retrievals
    EXPECT_NE(ba.asString(), "hello world");
}
