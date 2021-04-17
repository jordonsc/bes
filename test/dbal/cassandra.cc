#include <bes/dbal.h>
#include <gtest/gtest.h>

#include <vector>

using namespace bes::dbal::wide;

static char const* const TEST_SERVER = "localhost";
static char const* const TEST_SERVER_VERSION = "3.";  // test server should be Cassandra 3.x
static char const* const TEST_KEYSPACE = "test_ks";
static char const* const TEST_TABLE = "test_table";

static Schema createTestSchema()
{
    std::vector<Field> fields;
    fields.push_back({Datatype::Text, "test", "str"});
    fields.push_back({Datatype::Float32, "test", "flt"});

    return Schema({Datatype::Int32, "test", "pk"}, std::move(fields));
}

static bes::dbal::Context createContext()
{
    auto ctx = bes::dbal::Context();
    ctx.setParameter("hosts", TEST_SERVER);
    ctx.setParameter(cassandra::KEYSPACE_PARAM, TEST_KEYSPACE);

    return ctx;
}

TEST(CassandraTest, Connection)
{
    auto con = bes::dbal::wide::cassandra::Connection(createContext());
    ASSERT_TRUE(con.isConnected());
}

TEST(CassandraTest, ServerVersion)
{
    // Constructed via connection rvalue copy
    auto db = Cassandra(createContext());
    ASSERT_EQ(db.getServerVersion().substr(0, 2), TEST_SERVER_VERSION);
    ASSERT_EQ(db.getServerVersion().substr(0, 2), TEST_SERVER_VERSION);  // ensure consecutive queries work

    // Do some clean-up for subsequent tests, just in-case of bad/broken data from prior test runs
    db.setKeyspace(TEST_KEYSPACE);
    db.dropTable(TEST_TABLE, true);
    db.dropKeyspace(TEST_KEYSPACE, true);
}

TEST(CassandraTest, KeyspaceCreation)
{
    auto db = Cassandra(createContext());
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
    auto db = Cassandra(createContext());
    db.createKeyspace(cassandra::Keyspace(TEST_KEYSPACE), true);

    FieldList fields;
    fields.emplace_back(Datatype::Text, "test", "str");
    fields.emplace_back(Datatype::Float32, "test", "flt");

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
    auto db = Cassandra(createContext());
    db.setKeyspace(TEST_KEYSPACE);

    db.createKeyspace(cassandra::Keyspace(TEST_KEYSPACE), true);
    db.dropTable(TEST_TABLE, true);
    db.createTable(TEST_TABLE, createTestSchema(), false);

    // Create test data, nb that we are omitting `flt` here, which should be a null value
    ValueList v;
    v.push_back(Value("test", "str", "bar"));
    v.push_back(Value("test", "pk", 5));
    // NB: both insert and update are "upsert" statements, you could have used db.update() here, too
    db.insert(TEST_TABLE, std::move(v));

    auto result = db.retrieveTestData(TEST_TABLE, 5);

    EXPECT_EQ(result.rowCount(), 1);
    ASSERT_EQ(result.columnCount(), 3);

    // NB: PK then lexicographical ordering (I'm not sure how reliable this is)
    EXPECT_EQ(result.getColumn(0).ns, "test");
    EXPECT_EQ(result.getColumn(0).qualifier, "pk");
    EXPECT_EQ(result.getColumn(0).datatype, Datatype::Int32);

    EXPECT_EQ(result.getColumn(1).ns, "test");
    EXPECT_EQ(result.getColumn(1).qualifier, "flt");
    EXPECT_EQ(result.getColumn(1).datatype, Datatype::Float32);

    EXPECT_EQ(result.getColumn(2).ns, "test");
    EXPECT_EQ(result.getColumn(2).qualifier, "str");
    EXPECT_EQ(result.getColumn(2).datatype, Datatype::Text);

    // Using iterator
    size_t rows = 0;
    for (auto const& row : result) {
        ++rows;

        // lvalue (copy) from Cell
        auto pk = row[0];
        EXPECT_EQ(pk.getField().ns, "test");
        EXPECT_EQ(pk.getField().qualifier, "pk");
        EXPECT_EQ(pk.getField().datatype, Datatype::Int32);
        EXPECT_EQ(pk.as<Int32>(), 5);

        // rvalue (move) fro Cell
        EXPECT_EQ(std::move(pk).as<Int32>(), 5);

        // rvalue (entire row)
        EXPECT_EQ(row[0].as<Int32>(), 5);
        EXPECT_THROW(row[1].as<Float32>(), bes::dbal::NullValueException);
        EXPECT_EQ(row[2].as<Text>(), "bar");
    }
    EXPECT_EQ(rows, 1);

    // Using getFirstRow()
    size_t cols = 0;
    auto first_row = result.getFirstRow();

    // Value by index
    EXPECT_EQ(first_row[0].as<Int32>(), 5);

    // Value by name, unordered
    EXPECT_EQ(first_row.value<Text>("test", "str"), "bar");
    EXPECT_THROW(first_row.value<Float32>("test", "flt"), bes::dbal::NullValueException);
    EXPECT_EQ(first_row.value<Int32>("test", "pk"), 5);

    // Using a column iterator
    for (auto const& cell : first_row) {
        ++cols;
        auto const& f = cell.getField();
        EXPECT_EQ(f.ns, "test");

        if (f.qualifier == "pk") {
            ASSERT_EQ(f.datatype, Datatype::Int32);
            EXPECT_EQ(cell.as<Int32>(), 5);
        } else if (f.qualifier == "flt") {
            ASSERT_EQ(f.datatype, Datatype::Null);
            EXPECT_THROW(cell.as<Float32>(), bes::dbal::NullValueException);
        } else if (f.qualifier == "str") {
            ASSERT_EQ(f.datatype, Datatype::Text);
            EXPECT_EQ(cell.as<Text>(), "bar");
        } else {
            EXPECT_EQ(f.qualifier, "Qualifier did not match any expected fields");
        }
    }

    EXPECT_EQ(cols, 3);

    // Will update the table, adding a non-null value for `flt`
    v.clear();
    v.push_back(Value("test", "str", "hello world"));
    v.push_back(Value("test", "flt", (Float32)123.456));
    db.update(TEST_TABLE, Value("test", "pk", 5), std::move(v));

    result = db.retrieveTestData(TEST_TABLE, 5);
    first_row = result.getFirstRow();
    ASSERT_FLOAT_EQ(first_row.value<Float32>("test", "flt"), (Float32)123.456);
    ASSERT_EQ(first_row.value<Text>("test", "str"), "hello world");
}
