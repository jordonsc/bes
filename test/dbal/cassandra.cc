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
    fields.emplace_back(Datatype::Text, "test", "str");
    fields.emplace_back(Datatype::Float32, "test", "flt");

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
}

TEST(CassandraTest, KeyspaceCreation)
{
    auto db = Cassandra(createContext());
    db.setKeyspace(TEST_KEYSPACE);

    // Remove any traces of previous runs
    db.dropKeyspace(TEST_KEYSPACE, true).wait();

    cassandra::Keyspace ks(TEST_KEYSPACE);

    ASSERT_NO_THROW({
        db.createKeyspace(ks, false).wait();  // should succeed, keyspace doesn't exist
        db.createKeyspace(ks, true).wait();   // should succeed, keyspace exists but DB will skip
    });

    // Should fail, keyspace exists but DB will NOT skip
    ASSERT_THROW({ db.createKeyspace(ks, false).wait(); }, bes::dbal::DbalException);

    ASSERT_NO_THROW({
        db.dropKeyspace(TEST_KEYSPACE, false).wait();  // should succeed, keyspace exists
        db.dropKeyspace(TEST_KEYSPACE, true).wait();   // should succeed, keyspace exists but DB will skip
    });

    // Should fail, keyspace exists but DB will NOT skip
    ASSERT_THROW({ db.dropKeyspace(TEST_KEYSPACE, false).wait(); }, bes::dbal::DbalException);
}

TEST(CassandraTest, TableCreation)
{
    Context ctx;
    auto db = Cassandra(createContext());
    db.createKeyspace(cassandra::Keyspace(TEST_KEYSPACE), true).wait();

    FieldList fields;
    fields.emplace_back(Datatype::Text, "test", "str");
    fields.emplace_back(Datatype::Float32, "test", "flt");

    Schema s({Datatype::Int32, "test", "pk"}, std::move(fields));

    ASSERT_NO_THROW({
        db.createTable(TEST_TABLE, s, false).wait();
        db.createTable(TEST_TABLE, s, true).wait();
    });

    ASSERT_THROW({ db.createTable(TEST_TABLE, s, false).wait(); }, bes::dbal::DbalException);

    ASSERT_NO_THROW({
        db.dropTable(TEST_TABLE, false).wait();
        db.dropTable(TEST_TABLE, true).wait();
    });

    ASSERT_THROW({ db.dropKeyspace(TEST_TABLE, false).wait(); }, bes::dbal::DbalException);
}

TEST(CassandraTest, RowSemantics)
{
    auto db = Cassandra(createContext());
    db.setKeyspace(TEST_KEYSPACE);

    db.createKeyspace(cassandra::Keyspace(TEST_KEYSPACE), true).wait();
    db.dropTable(TEST_TABLE, true).wait();
    db.createTable(TEST_TABLE, createTestSchema(), false).wait();

    // Create test data, nb that we are omitting `flt` here, which should be a null value
    ValueList v;
    v.push_back(Value("test", "str", "bar"));
    v.push_back(Value("test", "pk", 5));
    // NB: both insert and update are "upsert" statements, you could have used db.update() here, too
    db.insert(TEST_TABLE, std::move(v)).wait();

    auto result = db.retrieve(TEST_TABLE, Value("test", "pk", (Int32)5));

    // NB: not waiting here, the first call to anything (eg rowCount()) will immediate force the future to wait()
    EXPECT_EQ(result.rowCount(), 1);
    ASSERT_EQ(result.columnCount(), 3);

    // Check the wrapper functions match the actual result object
    ASSERT_EQ(result.columnCount(), result.get()->columnCount());

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
    while (result.pop()) {
        ++rows;
        ASSERT_LT(rows, 5);  // infinite loop detection

        // lvalue (copy) from Cell
        auto pk = result.row()->at(0);
        EXPECT_EQ(pk.getField().ns, "test");
        EXPECT_EQ(pk.getField().qualifier, "pk");
        EXPECT_EQ(pk.getField().datatype, Datatype::Int32);
        EXPECT_EQ(pk.as<Int32>(), 5);

        // rvalue (move) fro Cell
        EXPECT_EQ(std::move(pk).as<Int32>(), 5);

        // rvalue (entire row) - using helper functions to reference the current row
        EXPECT_EQ(result[0].as<Int32>(), 5);
        EXPECT_THROW(result[1].as<Float32>(), bes::dbal::NullValueException);
        EXPECT_EQ(result[2].as<Text>(), "bar");
    }
    EXPECT_EQ(rows, 1);

    // Will update the table, adding a non-null value for `flt`
    v.clear();
    v.push_back(Value("test", "str", "hello world"));
    v.push_back(Value("test", "flt", (Float32)123.456));
    db.update(TEST_TABLE, Value("test", "pk", 5), std::move(v)).wait();

    result = db.retrieve(TEST_TABLE, Value("test", "pk", (Int32)5));
    Row const* row = result.pop();
    ASSERT_NE(row, nullptr);
    ASSERT_FLOAT_EQ(row->at("test", "flt").as<Float32>(), (Float32)123.456);
    ASSERT_EQ(row->at("test", "str").as<Text>(), "hello world");
}

TEST(CassandraTest, DataCreation)
{
    auto db = Cassandra(createContext());
    db.setKeyspace(TEST_KEYSPACE);

    db.createKeyspace(cassandra::Keyspace(TEST_KEYSPACE), true).wait();
    db.dropTable(TEST_TABLE, true).wait();
    db.createTable(TEST_TABLE, createTestSchema(), false).wait();

    // Table should be empty
    auto result = db.retrieve(TEST_TABLE, Value("test", "pk", (Int32)5));
    ASSERT_EQ(result.rowCount(), 0);
    EXPECT_EQ(result.pop(), nullptr);

    ValueList v;
    v.push_back(Value("test", "str", "bar"));
    v.push_back(Value("test", "flt", (Float32)123456.789));
    db.update(TEST_TABLE, Value("test", "pk", 5), std::move(v)).wait();

    v.clear();
    v.push_back(Value("test", "str", "bar"));
    v.push_back(Value("test", "flt", (Float32)7.5));
    v.push_back(Value("test", "pk", 90));
    db.insert(TEST_TABLE, std::move(v)).wait();

    // Cassandra should complain that we're attempting a table-scan (not using an indexed column)
    EXPECT_THROW(db.retrieve(TEST_TABLE, Value("test", "str", Text("bar"))).wait(), bes::dbal::DbalException);

    FieldList f;
    f.emplace_back("test", "str");
    f.emplace_back("test", "flt");

    result = db.retrieve(TEST_TABLE, Value("test", "pk", (Int32)5), std::move(f));
    EXPECT_EQ(result.rowCount(), 1);
    EXPECT_EQ(result.columnCount(), 2);

    db.truncate(TEST_TABLE).wait();

    result = db.retrieve(TEST_TABLE, Value("test", "pk", (Int32)5));
    EXPECT_EQ(result.rowCount(), 0);
    EXPECT_EQ(result.columnCount(), 3);
}

TEST(CassandraTest, Iterators)
{
    auto db = Cassandra(createContext());
    db.setKeyspace(TEST_KEYSPACE);

    db.createKeyspace(cassandra::Keyspace(TEST_KEYSPACE), true).wait();
    db.createTable(TEST_TABLE, createTestSchema(), true).wait();

    ValueList v;
    v.push_back(Value("test", "str", "nice iterator"));
    db.update(TEST_TABLE, Value("test", "pk", 1), std::move(v)).wait();

    auto result = db.retrieve(TEST_TABLE, Value("test", "pk", (Int32)1));

    // Creating an iterator will call wait() on the future
    for (auto const& row : result) {
        EXPECT_EQ(row.at("test", "str").as<Text>(), "nice iterator");
    }
}

TEST(CassandraTest, MultiKey)
{
    auto db = Cassandra(createContext());
    db.setKeyspace(TEST_KEYSPACE);

    db.createKeyspace(cassandra::Keyspace(TEST_KEYSPACE), true).wait();
    db.createTable(TEST_TABLE, createTestSchema(), true).wait();

    ValueList v;
    v.push_back(Value("test", "str", "row 1"));
    db.update(TEST_TABLE, Value("test", "pk", 101), std::move(v)).wait();

    v.clear();
    v.push_back(Value("test", "str", "row 2"));
    db.update(TEST_TABLE, Value("test", "pk", 102), std::move(v)).wait();

    v.clear();
    v.push_back(Value("test", "str", "row 3"));
    db.update(TEST_TABLE, Value("test", "pk", 103), std::move(v)).wait();

    auto pk_all = Value("test", "pk", Int32List({101, 102, 103}));
    auto result = db.retrieve(TEST_TABLE, pk_all);

    EXPECT_EQ(result.rowCount(), 3);
    size_t iterated_rows = 0;
    for (auto const& row : result) {
        EXPECT_NE(row.at("test", "str").as<Text>(), "awesome");
        ++iterated_rows;
    }
    EXPECT_EQ(iterated_rows, 3);

    auto pk_duo = Value("test", "pk", Int32List({102, 103}));
    db.update(TEST_TABLE, pk_duo, {Value("test", "str", "awesome")}).wait();

    result = db.retrieve(TEST_TABLE, pk_duo);
    for (auto const& row : result) {
        EXPECT_NE(row.at("test", "pk").as<Int32>(), 101);
        EXPECT_EQ(row.at("test", "str").as<Text>(), "awesome");
    }
}
