#include "connection.h"

#include "../exception.h"

using namespace bes::dbal::cassandra;

Connection::Connection(std::string hosts) : hosts(std::move(hosts))
{
    // Build new cluster under RAII control
    cluster.reset(cass_cluster_new(), [](CassCluster* r) {
        cass_cluster_free(r);
    });

    // Session
    session.reset(cass_session_new(), [](CassSession* r) {
        cass_session_free(r);
    });

    // Create the connection future, under RAII control
    cass_cluster_set_contact_points(cluster.get(), this->hosts.c_str());
    connect_future.reset(cass_session_connect(session.get(), cluster.get()), [](CassFuture* r) {
        cass_future_free(r);
    });

    // Will block until ready
    CassError rc = cass_future_error_code(connect_future.get());

    if (rc != CASS_OK) {
        /* Display connection error message */
        const char* message;
        size_t message_length;
        cass_future_error_message(connect_future.get(), &message, &message_length);
        auto err = std::string(message, message_length);

        throw DbalException(err);
    }

    connected = true;
}

std::shared_ptr<CassSession> Connection::GetSession() const
{
    return session;
}

CassSession* Connection::GetSessionPtr() const
{
    return session.get();
}

bool Connection::IsConnected() const
{
    return connected;
}
