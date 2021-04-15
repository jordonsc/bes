#ifndef BES_DBAL_WIDE_CASSANDRA_CONNECTION_H
#define BES_DBAL_WIDE_CASSANDRA_CONNECTION_H

#include <bes/log.h>

#include <memory>
#include <string>

#include "cassandra.h"

namespace bes::dbal::wide::cassandra {

class Connection
{
   public:
    /**
     * Create a new DB connection.
     *
     * By default, the Cassandra driver dumps to the console indiscriminately. If you opt to `own_logging`, we'll pipe
     * this into the Bes log system instead.
     */
    explicit Connection(std::string hosts, bool own_logging = true);

    [[nodiscard]] std::shared_ptr<CassSession> GetSession() const;
    [[nodiscard]] CassSession* GetSessionPtr() const;
    [[nodiscard]] bool IsConnected() const;

    static void DriverLog(CassLogMessage const* message, void* data);
    void* log_data = nullptr;

   protected:
    std::string hosts;
    std::shared_ptr<CassFuture> connect_future;
    std::shared_ptr<CassCluster> cluster;
    std::shared_ptr<CassSession> session;
    bool connected = false;

   private:
    static bes::log::Severity CassToBesSeverity(CassLogLevel);
};

}  // namespace bes::dbal::wide::cassandra

#endif