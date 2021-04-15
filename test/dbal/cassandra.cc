#include <bes/dbal.h>
#include <gtest/gtest.h>

#include <vector>

using namespace bes::dbal::wide;

static char const* const TEST_SERVER = "localhost";
static char const* const TEST_SERVER_VERSION = "3.11.10";
static char const* const TEST_KEYSPACE = "test_ks";
static char const* const TEST_TABLE = "test_table";

Schema CreateTestSchema()
{
    std::vector<Field> fields;
    fields.push_back({Datatype::Text, "test", "str"});
    fields.push_back({Datatype::Float32, "test", "flt"});

    return Schema({Datatype::Int32, "test", "pk"}, std::move(fields));
}

TEST(CassandraTest, Connection)
{
    auto con = bes::dbal::wide::cassandra::Connection(TEST_SERVER);
    ASSERT_TRUE(con.IsConnected());
}

TEST(CassandraTest, ServerVersion)
{
    // Constructed via connection rvalue copy
    auto db1 = Cassandra(cassandra::Connection(TEST_SERVER));
    ASSERT_EQ(db1.GetServerVersion(), TEST_SERVER_VERSION);
    ASSERT_EQ(db1.GetServerVersion(), TEST_SERVER_VERSION);  // ensure multiple queries work

    // Connection created by Cassandra class
    auto db2 = Cassandra(TEST_SERVER);
    ASSERT_EQ(db1.GetServerVersion(), TEST_SERVER_VERSION);

    // Do some clean-up for subsequent tests, just in-case of bad/broken data from prior test runs
    db1.SetKeyspace(TEST_KEYSPACE);
    db1.DropTable(TEST_TABLE, true);
    db1.DropKeyspace(TEST_KEYSPACE, true);
}

TEST(CassandraTest, KeyspaceCreation)
{
    auto db = Cassandra(cassandra::Connection(TEST_SERVER));
    db.SetKeyspace(TEST_KEYSPACE);

    // Remove any traces of previous runs
    db.DropKeyspace(TEST_KEYSPACE, true);

    cassandra::Keyspace ks(TEST_KEYSPACE);

    ASSERT_NO_THROW({
        db.CreateKeyspace(ks, false);  // should succeed, keyspace doesn't exist
        db.CreateKeyspace(ks, true);   // should succeed, keyspace exists but DB will skip
    });

    // Should fail, keyspace exists but DB will NOT skip
    ASSERT_THROW({ db.CreateKeyspace(ks, false); }, bes::dbal::DbalException);

    ASSERT_NO_THROW({
        db.DropKeyspace(TEST_KEYSPACE, false);  // should succeed, keyspace exists
        db.DropKeyspace(TEST_KEYSPACE, true);   // should succeed, keyspace exists but DB will skip
    });

    // Should fail, keyspace exists but DB will NOT skip
    ASSERT_THROW({ db.DropKeyspace(TEST_KEYSPACE, false); }, bes::dbal::DbalException);
}

TEST(CassandraTest, TableCreation)
{
    Context ctx;
    ctx.SetParameter(cassandra::KEYSPACE_PARAM, TEST_KEYSPACE);
    auto db = Cassandra(cassandra::Connection(TEST_SERVER), std::move(ctx));
    db.CreateKeyspace(cassandra::Keyspace(TEST_KEYSPACE), true);

    std::vector<Field> fields;
    fields.push_back({Datatype::Text, "test", "str"});
    fields.push_back({Datatype::Float32, "test", "flt"});

    Schema s({Datatype::Int32, "test", "pk"}, std::move(fields));

    ASSERT_NO_THROW({
        db.CreateTable(TEST_TABLE, s, false);
        db.CreateTable(TEST_TABLE, s, true);
    });

    ASSERT_THROW({ db.CreateTable(TEST_TABLE, s, false); }, bes::dbal::DbalException);

    ASSERT_NO_THROW({
        db.DropTable(TEST_TABLE, false);
        db.DropTable(TEST_TABLE, true);
    });

    ASSERT_THROW({ db.DropKeyspace(TEST_TABLE, false); }, bes::dbal::DbalException);
}

TEST(CassandraTest, RowCreation)
{
    auto db = Cassandra(TEST_SERVER);
    db.SetKeyspace(TEST_KEYSPACE);

    db.CreateKeyspace(cassandra::Keyspace(TEST_KEYSPACE), true);
    db.DropTable(TEST_TABLE, true);
    db.CreateTable(TEST_TABLE, CreateTestSchema(), false);

    db.CreateTestData(TEST_TABLE, 5, "bar");
    auto bar = db.RetrieveTestData(TEST_TABLE, 5);

    ASSERT_EQ(bar, "bar");
}
