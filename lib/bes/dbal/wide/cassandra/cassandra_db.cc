#include "cassandra_db.h"

#include <shared_mutex>
#include <utility>

using namespace bes::dbal::wide;
using cassandra::Connection;

Cassandra::Cassandra(cassandra::Connection&& connection) : connection(connection) {}
Cassandra::Cassandra(std::string hosts) : connection(Connection(std::move(hosts))) {}

Cassandra::Cassandra(cassandra::Connection&& con, Context&& c) : WideColumnDb(c), connection(std::move(con)) {}
Cassandra::Cassandra(std::string hosts, Context&& c) : WideColumnDb(c), connection(Connection(std::move(hosts))) {}

Cassandra::Cassandra(cassandra::Connection&& con, Context const& c) : WideColumnDb(c), connection(std::move(con)) {}
Cassandra::Cassandra(std::string hosts, Context const& c) : WideColumnDb(c), connection(Connection(std::move(hosts))) {}

std::string Cassandra::GetServerVersion() const
{
    ValidateConnection();

    // Build statement and execute query
    CassStatement* statement = cass_statement_new("SELECT release_version FROM system.local", 0);
    CassFuture* query_future = cass_session_execute(connection.GetSessionPtr(), statement);

    try {
        if (cass_future_error_code(query_future) == CASS_OK) {
            // Retrieve result set and get the first row
            const CassResult* result = cass_future_get_result(query_future);
            const CassRow* row = cass_result_first_row(result);

            if (row) {
                const CassValue* value = cass_row_get_column_by_name(row, "release_version");
                const char* release_version;
                size_t release_version_length;
                cass_value_get_string(value, &release_version, &release_version_length);

                auto out = std::string(release_version, release_version_length);

                cass_statement_free(statement);
                cass_result_free(result);
                cass_future_free(query_future);

                return out;
            } else {
                cass_statement_free(statement);
                cass_result_free(result);
                cass_future_free(query_future);

                return "Unknown (no row)";
            }

        } else {
            const char* message;
            size_t message_length;
            cass_future_error_message(query_future, &message, &message_length);
            throw DbalException(std::string(message, message_length));
        }

    } catch (std::exception const& e) {
        cass_future_free(query_future);
        throw DbalException(e.what());
    }
}

void Cassandra::SetKeyspace(std::string const& value)
{
    std::unique_lock lock(ks_mutex);
    keyspace = value;
}

std::string const& Cassandra::GetKeyspace() const
{
    std::shared_lock lock(ks_mutex);

    if (keyspace.empty()) {
        // Keyspace not defined, will need to check if it exists in the context
        try {
            auto const& ks = context.GetParameter(cassandra::KEYSPACE_PARAM);
            keyspace = ks;
            return keyspace;
        } catch (std::exception const&) {
            throw DbalException("Cassandra keyspace is not defined");
        }
    } else {
        return keyspace;
    }
}

void Cassandra::ExecuteQuerySync(std::string const& cql) const
{
    // TODO: turn this into an RAII-controlled object
    CassError rc;
    CassFuture* future;
    CassStatement* statement = cass_statement_new(cql.c_str(), 0);

    future = cass_session_execute(connection.GetSessionPtr(), statement);

    try {
        cass_future_wait(future);

        rc = cass_future_error_code(future);
        if (rc != CASS_OK) {
            throw DbalException(GetFutureErrMsg(future));
        }

    } catch (std::exception const& e) {
        cass_future_free(future);
        cass_statement_free(statement);
        throw DbalException(std::string("Error executing query: ").append(e.what()));
    }

    cass_future_free(future);
    cass_statement_free(statement);
}

void Cassandra::CreateKeyspace(cassandra::Keyspace const& ks, bool if_not_exists) const
{
    /*
     * CREATE  KEYSPACE [IF NOT EXISTS] keyspace_name
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

    ExecuteQuerySync(cql);
}

void Cassandra::DropKeyspace(const std::string& ks_name, bool if_exists) const
{
    /*
     * DROP KEYSPACE [IF EXISTS] keyspace_name;
     */
    std::string cql = if_exists ? "DROP KEYSPACE IF EXISTS " : "DROP KEYSPACE ";
    cql.append(ks_name);

    ExecuteQuerySync(cql);
}

void Cassandra::CreateTable(std::string const& table_name, Schema const& schema, bool if_not_exists) const
{
    std::string cql = if_not_exists ? "CREATE TABLE IF NOT EXISTS " : "CREATE TABLE ";
    cql.append(GetKeyspace())
        .append(".")
        .append(table_name)
        .append(" (")
        .append(GetFieldCql(schema.primary_key))
        .append(" PRIMARY KEY");
    for (auto const& f : schema.fields) {
        cql.append(", ").append(GetFieldCql(f));
    }
    cql.append(");");

    ExecuteQuerySync(cql);
}

void Cassandra::DropTable(std::string const& table_name, bool if_exists) const {}

[[nodiscard]] const char* Cassandra::FieldType(Datatype const& dt)
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

std::string Cassandra::GetFieldCql(Field const& f)
{
    std::string r = f.ns;
    r.append("_").append(f.qualifier).append(" ").append(FieldType(f.datatype));

    return r;
}

void Cassandra::ValidateConnection() const
{
    if (!connection.IsConnected()) {
        throw NotConnectedException("Not connected to server");
    }
}

std::string Cassandra::GetFutureErrMsg(CassFuture* future)
{
    const char* message;
    size_t message_length;
    cass_future_error_message(future, &message, &message_length);

    return std::string(message, message_length);
}
