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
    ASSERT_TRUE(con.isConnected());
}

TEST(CassandraTest, ServerVersion)
{
    // Constructed via connection rvalue copy
    auto db1 = Cassandra(cassandra::Connection(TEST_SERVER));
    ASSERT_EQ(db1.getServerVersion(), TEST_SERVER_VERSION);
    ASSERT_EQ(db1.getServerVersion(), TEST_SERVER_VERSION);  // ensure multiple queries work

    // Connection created by Cassandra class
    auto db2 = Cassandra(TEST_SERVER);
    ASSERT_EQ(db1.getServerVersion(), TEST_SERVER_VERSION);

    // Do some clean-up for subsequent tests, just in-case of bad/broken data from prior test runs
    db1.setKeyspace(TEST_KEYSPACE);
    db1.dropTable(TEST_TABLE, true);
    db1.dropKeyspace(TEST_KEYSPACE, true);
}

TEST(CassandraTest, KeyspaceCreation)
{
    auto db = Cassandra(cassandra::Connection(TEST_SERVER));
    db.setKeyspace(TEST_KEYSPACE);

    // Remove any traces of previous runs
    db.dropKeyspace(TEST_KEYSPACE, true);

    cassandra::Keyspace ks(TEST_KEYSPACE);

    ASSERT_NO_THROW({
        db.createKeyspace(ks, false);  // should succeed, keyspace doesn't exist
        db.createKeyspace(ks, true);   // should succeed, keyspace exists but DB will skip
    });

    // Should fail, keyspace exists but DB will NOT skip
    ASSERT_THROW({ db.createKeyspace(ks, false); }, bes::dbal::DbalException);

    ASSERT_NO_THROW({
        db.dropKeyspace(TEST_KEYSPACE, false);  // should succeed, keyspace exists
        db.dropKeyspace(TEST_KEYSPACE, true);   // should succeed, keyspace exists but DB will skip
    });

    // Should fail, keyspace exists but DB will NOT skip
    ASSERT_THROW({ db.dropKeyspace(TEST_KEYSPACE, false); }, bes::dbal::DbalException);
}

TEST(CassandraTest, TableCreation)
{
    Context ctx;
    ctx.SetParameter(cassandra::KEYSPACE_PARAM, TEST_KEYSPACE);
    auto db = Cassandra(cassandra::Connection(TEST_SERVER), std::move(ctx));
    db.createKeyspace(cassandra::Keyspace(TEST_KEYSPACE), true);

    std::vector<Field> fields;
    fields.push_back({Datatype::Text, "test", "str"});
    fields.push_back({Datatype::Float32, "test", "flt"});

    Schema s({Datatype::Int32, "test", "pk"}, std::move(fields));

    ASSERT_NO_THROW({
        db.createTable(TEST_TABLE, s, false);
        db.createTable(TEST_TABLE, s, true);
    });

    ASSERT_THROW({ db.createTable(TEST_TABLE, s, false); }, bes::dbal::DbalException);

    ASSERT_NO_THROW({
        db.dropTable(TEST_TABLE, false);
        db.dropTable(TEST_TABLE, true);
    });

    ASSERT_THROW({ db.dropKeyspace(TEST_TABLE, false); }, bes::dbal::DbalException);
}

TEST(CassandraTest, RowCreation)
{
    auto db = Cassandra(TEST_SERVER);
    db.setKeyspace(TEST_KEYSPACE);

    db.createKeyspace(cassandra::Keyspace(TEST_KEYSPACE), true);
    db.dropTable(TEST_TABLE, true);
    db.createTable(TEST_TABLE, CreateTestSchema(), false);

    db.createTestData(TEST_TABLE, 5, "bar");
    auto result = db.retrieveTestData(TEST_TABLE, 5);

    ASSERT_EQ(result.rowCount(), 1);
    ASSERT_EQ(result.columnCount(), 3);

    // NB: PK then lexicographical ordering (I'm not sure how reliable this is)
    ASSERT_EQ(result.getColumn(0).ns, "test");
    ASSERT_EQ(result.getColumn(0).qualifier, "pk");
    ASSERT_EQ(result.getColumn(0).datatype, Datatype::Int32);

    ASSERT_EQ(result.getColumn(1).ns, "test");
    ASSERT_EQ(result.getColumn(1).qualifier, "flt");
    ASSERT_EQ(result.getColumn(1).datatype, Datatype::Float32);

    ASSERT_EQ(result.getColumn(2).ns, "test");
    ASSERT_EQ(result.getColumn(2).qualifier, "str");
    ASSERT_EQ(result.getColumn(2).datatype, Datatype::Text);



}
