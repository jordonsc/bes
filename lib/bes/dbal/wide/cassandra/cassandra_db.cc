#include "cassandra_db.h"

#include <utility>

using namespace bes::dbal::wide;

Cassandra::Cassandra(cassandra::Connection&& connection) : connection(connection) {}
Cassandra::Cassandra(std::string hosts) : connection(cassandra::Connection(std::move(hosts))) {}

std::string Cassandra::GetServerVersion()
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

void Cassandra::ExecuteQuerySync(const std::string& cql)
{
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

void Cassandra::CreateTable(std::string table_name, Schema schema, bool if_not_exists)
{
    std::string cql = if_not_exists ? "CREATE TABLE IF NOT EXISTS " : "CREATE TABLE ";
    cql.append(table_name).append(" (").append(GetFieldCql(schema.primary_key)).append(" PRIMARY KEY");
    for (auto const& f : schema.fields) {
        cql.append(", ").append(GetFieldCql(f));
    }
    cql.append(");");

    ExecuteQuerySync(cql);
}

void Cassandra::DropTable(std::string table_name, bool if_exists) {}

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
    r.append(".").append(f.qualifier).append(" ").append(FieldType(f.datatype));
    return r;
}

void Cassandra::ValidateConnection()
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
