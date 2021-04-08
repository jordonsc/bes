#include <bes/dbal.h>
#include <gtest/gtest.h>

struct test_svr
{
    static std::string const hostname;
    static std::string const version;
};

std::string const test_svr::hostname = "localhost";
std::string const test_svr::version = "3.11.10";

TEST(CassandraTest, Connection)
{
    auto con = bes::dbal::cassandra::Connection(test_svr::hostname);
    ASSERT_TRUE(con.IsConnected());
}

TEST(CassandraTest, ServerVersion)
{
    // Constructed via connection rvalue copy
    auto db1 = bes::dbal::Cassandra(bes::dbal::cassandra::Connection(test_svr::hostname));
    ASSERT_EQ(db1.GetServerVersion(), test_svr::version);
    ASSERT_EQ(db1.GetServerVersion(), test_svr::version);  // ensure multiple queries work

    // Connection created by Cassandra class
    auto db2 = bes::dbal::Cassandra("localhost");
    ASSERT_EQ(db1.GetServerVersion(), test_svr::version);
}
