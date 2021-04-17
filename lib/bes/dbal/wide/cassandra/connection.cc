#include "connection.h"

#include "../../exception.h"

using namespace bes::dbal::wide::cassandra;

Connection::Connection(Context const& ctx, bool own_logging)
{
    if (own_logging) {
        // Pipe the Cassandra driver logging into the Bes logger
        cass_log_set_level(CASS_LOG_TRACE);
        cass_log_set_callback(Connection::driverLog, this->log_data);
    }

    try {
        hosts = ctx.getParameter("hosts");
    } catch (OutOfRangeException const&) {
        throw DbalException("Cassandra connection requires a 'host' context parameter");
    }

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

std::shared_ptr<CassSession> Connection::getSession() const
{
    return session;
}

CassSession* Connection::getSessionPtr() const
{
    return session.get();
}

bool Connection::isConnected() const
{
    return connected;
}

void Connection::driverLog(CassLogMessage const* message, void* data)
{
    BES_LOG_LVL(Connection::cassToBesSeverity(message->severity)) << "CASS: " << message->message;
}

bes::log::Severity Connection::cassToBesSeverity(CassLogLevel s)
{
    switch (s) {
        default:
        case CASS_LOG_INFO:
            return bes::log::Severity::INFO;
        case CASS_LOG_TRACE:
            return bes::log::Severity::TRACE;
        case CASS_LOG_DEBUG:
            return bes::log::Severity::DEBUG;
        case CASS_LOG_WARN:
            return bes::log::Severity::WARNING;
        case CASS_LOG_ERROR:
            return bes::log::Severity::ERROR;
        case CASS_LOG_CRITICAL:
            return bes::log::Severity::CRITICAL;
    }
}
