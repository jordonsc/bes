#include "cassandra_db.h"

#include <shared_mutex>
#include <utility>

#include "result.h"
#include "types.h"

using namespace bes::dbal::wide;
using cassandra::Connection;

Cassandra::Cassandra(Context c) : WideColumnDb(std::move(c)), connection(getContext()) {}

std::string Cassandra::getServerVersion() const
{
    validateConnection();
    return execute(cassandra::Query("SELECT release_version FROM system.local")).pop()->at(0).as<Text>();
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

ResultFuture Cassandra::createKeyspace(cassandra::Keyspace const& ks, bool if_not_exists) const
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

    return execute(cassandra::Query(cql));
}

ResultFuture Cassandra::dropKeyspace(const std::string& ks_name, bool if_exists) const
{
    /*
     * DROP KEYSPACE [IF EXISTS] keyspace_name;
     */
    std::string cql = if_exists ? "DROP KEYSPACE IF EXISTS " : "DROP KEYSPACE ";
    cql.append(ks_name);

    return execute(cassandra::Query(cql));
}

ResultFuture Cassandra::createTable(std::string const& table_name, Schema const& schema, bool if_not_exists) const
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

    return execute(cassandra::Query(cql));
}

ResultFuture Cassandra::dropTable(std::string const& table_name, bool if_exists) const
{
    /*
     * DROP TABLE [IF EXISTS] keyspace.table_name;
     */
    std::string cql = if_exists ? "DROP TABLE IF EXISTS " : "DROP TABLE ";
    cql.append(getKeyspace()).append(".").append(table_name);

    return execute(cassandra::Query(cql));
}

void Cassandra::validateConnection() const
{
    if (!connection.isConnected()) {
        throw NotConnectedException("Not connected to server");
    }
}

ResultFuture Cassandra::insert(std::string const& t, ValueList values) const
{
    /*
     * INSERT INTO keyspace.table_name (field [,field]) VALUES (? [,?]);
     */
    if (values.empty()) {
        throw DbalException("Cannot update without any values");
    }

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
    cql.append(");");

    cassandra::Query q(cql, values.size());

    // Bind values
    for (auto& v : values) {
        bindValue(q, std::move(v));
    }

    return execute(std::move(q));
}

ResultFuture Cassandra::update(std::string const& t, Value const& key, ValueList values) const
{
    /*
     * UPDATE keyspace.table_name SET field = ? [, field = ?] WHERE field = ?;
     */
    if (values.empty()) {
        throw DbalException("Cannot update without any values");
    }

    auto cql = std::string("UPDATE ");
    cql.append(getKeyspace()).append(".").append(t).append(" SET ");

    bool first = true;
    for (auto const& v : values) {
        if (first) {
            first = false;
        } else {
            cql.append(", ");
        }
        cql.append(getFieldCql(v, false)).append(" = ?");
    }

    cql.append(" WHERE ");
    cql.append(getFieldCql(key, false));
    cql.append(" = ?;");

    cassandra::Query q(cql, values.size() + 1);

    // Bind values
    for (auto& v : values) {
        bindValue(q, std::move(v));
    }

    // Bind PK
    bindValue(q, key);

    return execute(std::move(q));
}

ResultFuture Cassandra::retrieve(std::string const& table_name, Value const& key) const
{
    auto cql = std::string("SELECT * FROM ");
    cql.append(getKeyspace()).append(".").append(table_name).append(" WHERE ");
    cql.append(getFieldCql(key)).append(" = ?;");

    cassandra::Query q(cql, 1);
    bindValue(q, key);

    return execute(std::move(q));
}

ResultFuture Cassandra::retrieve(std::string const& table_name, Value const& key, FieldList fields) const
{
    auto cql = std::string("SELECT ");

    bool first = true;
    for (auto const& f : fields) {
        if (first) {
            first = false;
        } else {
            cql.append(", ");
        }
        cql.append(getFieldCql(f));
    }

    cql.append(" FROM ");
    cql.append(getKeyspace()).append(".").append(table_name).append(" WHERE ");
    cql.append(getFieldCql(key)).append(" = ?;");

    cassandra::Query q(cql, 1);
    bindValue(q, key);

    return execute(std::move(q));
}

ResultFuture Cassandra::remove(std::string const& table_name, Value const& key) const
{
    auto cql = std::string("DELETE FROM ");
    cql.append(getKeyspace()).append(".").append(table_name).append(" WHERE ");
    cql.append(getFieldCql(key)).append(" = ?;");

    cassandra::Query q(cql, 1);
    bindValue(q, key);

    return execute(std::move(q));
}

ResultFuture Cassandra::truncate(std::string const& table_name) const
{
    auto cql = std::string("TRUNCATE TABLE ");
    cql.append(getKeyspace()).append(".").append(table_name).append(";");

    return execute(cassandra::Query(cql));
}

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

ResultFuture Cassandra::execute(cassandra::Query q) const
{
    q.execute(connection);
    return ResultFuture(std::make_shared<bes::dbal::wide::cassandra::Result>(std::move(q)));
}
