#include "cassandra_db.h"

#include <utility>

using namespace bes::dbal;

Cassandra::Cassandra(cassandra::Connection&& connection) : connection(connection) {}
Cassandra::Cassandra(std::string hosts) : connection(cassandra::Connection(std::move(hosts))) {}

void Cassandra::ValidateConnection()
{
    if (!connection.IsConnected()) {
        throw NotConnectedException("Not connected to server");
    }
}

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
