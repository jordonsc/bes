#include "cassandra_db.h"

#include <shared_mutex>
#include <utility>

#include "types.h"

using namespace bes::dbal::wide;
using cassandra::Connection;

Cassandra::Cassandra(Context c) : WideColumnDb(std::move(c)), connection(getContext()) {}

std::string Cassandra::getServerVersion() const
{
    validateConnection();

    return cassandra::Query("SELECT release_version FROM system.local")
        .getResult(connection)
        .getFirstRow()[0]
        .as<std::string>();
}

void Cassandra::setKeyspace(std::string const& value)
{
    std::unique_lock lock(ks_mutex);
    keyspace = value;
}

std::string const& Cassandra::getKeyspace() const
{
    std::shared_lock lock(ks_mutex);

    if (keyspace.empty()) {
        // Keyspace not defined, will need to check if it exists in the context
        try {
            auto const& ks = getContext().getParameter(cassandra::KEYSPACE_PARAM);
            keyspace = ks;
            return keyspace;
        } catch (std::exception const&) {
            throw DbalException("Cassandra keyspace is not defined");
        }
    } else {
        return keyspace;
    }
}

void Cassandra::createKeyspace(cassandra::Keyspace const& ks, bool if_not_exists) const
{
    /*
     * CREATE KEYSPACE [IF NOT EXISTS] keyspace_name
     * WITH REPLICATION = {
     *    'class' : 'SimpleStrategy', 'replication_factor' : N
     *   | 'class' : 'NetworkTopologyStrategy',
     *     'dc1_name' : N [, ...]
     * }
     * [AND DURABLE_WRITES =  true|false] ;
     */
    std::string cql = if_not_exists ? "CREATE KEYSPACE IF NOT EXISTS " : "CREATE KEYSPACE ";
    cql.append(ks.name).append(" WITH REPLICATION = {'class': '");
    switch (ks.replication_strategy) {
        default:
        case cassandra::ReplicationStrategy::SIMPLE:
            cql.append("SimpleStrategy");
            break;
        case cassandra::ReplicationStrategy::NETWORK_TOPOLOGY:
            cql.append("NetworkTopologyStrategy");
            break;
    }
    cql.append("', 'replication_factor': ").append(std::to_string(ks.replication_factor));

    if (ks.replication_strategy == cassandra::ReplicationStrategy::NETWORK_TOPOLOGY && !ks.dc_replication.empty()) {
        for (auto const& it : ks.dc_replication) {
            cql.append(", ").append(it.first).append(": ").append(std::to_string(it.second));
        }
    }
    cql.append("}");

    if (ks.replication_strategy != cassandra::ReplicationStrategy::SIMPLE) {
        if (ks.durable_writes) {
            cql.append(" AND DURABLE_WRITES = true");
        } else {
            cql.append(" AND DURABLE_WRITES = false");
        }
    }

    cql.append(";");

    cassandra::Query q(cql);
    q.executeSync(connection);
}

void Cassandra::dropKeyspace(const std::string& ks_name, bool if_exists) const
{
    /*
     * DROP KEYSPACE [IF EXISTS] keyspace_name;
     */
    std::string cql = if_exists ? "DROP KEYSPACE IF EXISTS " : "DROP KEYSPACE ";
    cql.append(ks_name);

    cassandra::Query q(cql);
    q.executeSync(connection);
}

void Cassandra::createTable(std::string const& table_name, Schema const& schema, bool if_not_exists) const
{
    /*
     * CREATE TABLE [IF NOT EXISTS] keyspace.table_name (
     *    field_name field_type [PRIMARY KEY],
     *    field_name field_type ...
     * );
     */
    std::string cql = if_not_exists ? "CREATE TABLE IF NOT EXISTS " : "CREATE TABLE ";
    cql.append(getKeyspace())
        .append(".")
        .append(table_name)
        .append(" (")
        .append(getFieldCql(schema.primary_key, true))
        .append(" PRIMARY KEY");
    for (auto const& f : schema.fields) {
        cql.append(", ").append(getFieldCql(f, true));
    }
    cql.append(");");

    cassandra::Query q(cql);
    q.executeSync(connection);
}

void Cassandra::dropTable(std::string const& table_name, bool if_exists) const
{
    /*
     * DROP TABLE [IF EXISTS] keyspace.table_name;
     */
    std::string cql = if_exists ? "DROP TABLE IF EXISTS " : "DROP TABLE ";
    cql.append(getKeyspace()).append(".").append(table_name);

    cassandra::Query q(cql);
    q.executeSync(connection);
}

void Cassandra::validateConnection() const
{
    if (!connection.isConnected()) {
        throw NotConnectedException("Not connected to server");
    }
}

/**
 * Test function.
 *
 * @deprecated delete me.
 */
void Cassandra::createTestData(std::string const& tbl, int a, std::string const& b) const
{
    ValueList v;
    v.push_back(Value("test", "str", b));
    
    // Tests expect a null value for `flt`
    //v.push_back(Value("test", "flt", (float)123.456));

    v.push_back(Value("test", "pk", a));

    update(tbl, std::move(v));
}

/**
 * Test function.
 *
 * @deprecated delete me.
 */
cassandra::ResultT Cassandra::retrieveTestData(std::string const& tbl, int a) const
{
    auto cql = std::string("SELECT * FROM ");
    cql.append(getKeyspace()).append(".").append(tbl).append(" WHERE test_pk = ?;");

    cassandra::Query q(cql, 1);
    q.bind(a);

    return q.getResult(connection);
}

void Cassandra::update(std::string const& t, ValueList values) const
{
    update(t, Value(), std::move(values));
}

void Cassandra::update(std::string const& t, Value const& key, ValueList values) const
{
    /*
     * INSERT INTO keyspace.table_name (field [, field]) VALUES (? [, ?])
     * WHERE field = ?;
     */
    if (values.empty()) {
        throw DbalException("Cannot update without any values");
    }

    size_t args = values.size();

    auto cql = std::string("INSERT INTO ");
    cql.append(getKeyspace()).append(".").append(t).append(" (");

    bool first = true;
    for (auto const& v : values) {
        if (first) {
            first = false;
        } else {
            cql.append(", ");
        }
        cql.append(getFieldCql(v, false));
    }
    cql.append(") VALUES (?");
    for (size_t i = 1; i < values.size(); ++i) {
        cql.append(", ?");
    }

    if (key.datatype == Datatype::Null) {
        // No PK (typical insert)
        cql.append(");");
    } else {
        // PK - include a where clause
        cql.append(") WHERE ");
        cql.append(getFieldCql(key, false));
        cql.append(" = ?;");
        ++args;
    }

    cassandra::Query q(cql, args);

    for (auto& v : values) {
        bindValue(q, std::move(v));
    }

    if (key.datatype != Datatype::Null) {
        bindValue(q, key);
    }

    q.executeSync(connection);
}

void Cassandra::remove(std::string const& table_name, Value const& key) const {}

void Cassandra::retrieve(std::string const& table_name, Value const& key) const {}

void Cassandra::bindValue(cassandra::Query& q, Value v)
{
    switch (v.datatype) {
        default:
            throw DbalException("Unknown datatype in update request (" + v.ns + cassandra::NS_DELIMITER + v.qualifier +
                                ")");
        case Datatype::Null:
            q.bind();
            break;
        case Datatype::Text:
            q.bind(std::move(std::any_cast<Text&&>(std::move(v.value))));
            break;
        case Datatype::Boolean:
            q.bind(std::any_cast<Boolean>(v.value));
            break;
        case Datatype::Int32:
            q.bind(std::any_cast<Int32>(v.value));
            break;
        case Datatype::Int64:
            q.bind(std::any_cast<Int64>(v.value));
            break;
        case Datatype::Float32:
            q.bind(std::any_cast<Float32>(v.value));
            break;
        case Datatype::Float64:
            q.bind(std::any_cast<Float64>(v.value));
            break;
    }
}

[[nodiscard]] const char* Cassandra::fieldType(Datatype const& dt)
{
    switch (dt) {
        case Datatype::Text:
            return "text";
        case Datatype::Boolean:
            return "boolean";
        case Datatype::Int32:
            return "int";
        case Datatype::Int64:
            return "bigint";
        case Datatype::Float32:
            return "float";
        case Datatype::Float64:
            return "double";
        default:
            throw bes::dbal::DbalException("Unknown datatype (programmatic error, table requires updating)");
    }
}

std::string Cassandra::getFieldCql(Field const& f, bool with_field_type)
{
    std::string r;
    if (!f.ns.empty()) {
        r.append(f.ns);
        r += cassandra::NS_DELIMITER;
    }
    r.append(f.qualifier);

    if (f.qualifier.empty()) {
        throw DbalException("Missing field qualifier");
    }

    if (with_field_type) {
        r.append(" ").append(fieldType(f.datatype));
    }

    return r;
}

std::string Cassandra::getFieldCql(Value const& v, bool with_field_type)
{
    std::string r;
    if (!v.ns.empty()) {
        r.append(v.ns);
        r += cassandra::NS_DELIMITER;
    }
    r.append(v.qualifier);

    if (v.qualifier.empty()) {
        throw DbalException("Missing field qualifier");
    }

    if (with_field_type) {
        r.append(" ").append(fieldType(v.datatype));
    }

    return r;
}
