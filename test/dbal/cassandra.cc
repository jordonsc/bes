#include <bes/dbal.h>
#include <gtest/gtest.h>

#include <vector>

using namespace bes::dbal::wide;

struct test_svr
{
    static std::string const hostname;
    static std::string const version;
};

std::string const test_svr::hostname("localhost");
std::string const test_svr::version("3.11.10");

static char const* const TEST_KS_NAME = "test_ks";

TEST(CassandraTest, Connection)
{
    auto con = bes::dbal::wide::cassandra::Connection(test_svr::hostname);
    ASSERT_TRUE(con.IsConnected());
}

TEST(CassandraTest, ServerVersion)
{
    // Constructed via connection rvalue copy
    auto db1 = Cassandra(cassandra::Connection(test_svr::hostname));
    ASSERT_EQ(db1.GetServerVersion(), test_svr::version);
    ASSERT_EQ(db1.GetServerVersion(), test_svr::version);  // ensure multiple queries work

    // Connection created by Cassandra class
    auto db2 = Cassandra("localhost");
    ASSERT_EQ(db1.GetServerVersion(), test_svr::version);
}

TEST(CassandraTest, KeyspaceCreation)
{
    auto db = Cassandra(cassandra::Connection(test_svr::hostname));

    // Remove any traces of previous runs
    db.DropKeyspace(TEST_KS_NAME, true);

    cassandra::Keyspace ks(TEST_KS_NAME);

    ASSERT_NO_THROW({
        db.CreateKeyspace(ks, false);  // should succeed, keyspace doesn't exist
        db.CreateKeyspace(ks, true);   // should succeed, keyspace exists but DB will skip
    });

    // Should fail, keyspace exists but DB will NOT skip
    ASSERT_THROW({ db.CreateKeyspace(ks, false); }, bes::dbal::DbalException);

    ASSERT_NO_THROW({
        db.DropKeyspace(TEST_KS_NAME, false);  // should succeed, keyspace exists
        db.DropKeyspace(TEST_KS_NAME, true);   // should succeed, keyspace exists but DB will skip
    });

    // Should fail, keyspace exists but DB will NOT skip
    ASSERT_THROW({ db.DropKeyspace(TEST_KS_NAME, false); }, bes::dbal::DbalException);
}

TEST(CassandraTest, TableCreation)
{
    std::string test_table("test_table");

    Context ctx;
    ctx.SetParameter(cassandra::KEYSPACE_PARAM, TEST_KS_NAME);
    auto db = Cassandra(cassandra::Connection(test_svr::hostname), std::move(ctx));
    db.CreateKeyspace(cassandra::Keyspace(TEST_KS_NAME), true);

    std::vector<Field> fields;
    fields.push_back({Datatype::Text, "test", "str"});
    fields.push_back({Datatype::Float32, "test", "flt"});

    Schema s({Datatype::Int32, "test", "pk"}, std::move(fields));

    ASSERT_NO_THROW({
        db.CreateTable(test_table, s, false);
        db.CreateTable(test_table, s, true);
    });

    ASSERT_THROW({ db.CreateTable(test_table, s, false); }, bes::dbal::DbalException);

    ASSERT_NO_THROW({
        db.DropTable(test_table, false);
        db.DropTable(test_table, true);
    });

    ASSERT_THROW({ db.DropKeyspace(test_table, false); }, bes::dbal::DbalException);
}
