#pragma once

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

    [[nodiscard]] std::shared_ptr<CassSession> getSession() const;
    [[nodiscard]] CassSession* getSessionPtr() const;
    [[nodiscard]] bool isConnected() const;

    static void driverLog(CassLogMessage const* message, void* data);
    void* log_data = nullptr;

   protected:
    std::string hosts;
    std::shared_ptr<CassFuture> connect_future;
    std::shared_ptr<CassCluster> cluster;
    std::shared_ptr<CassSession> session;
    bool connected = false;

   private:
    static bes::log::Severity cassToBesSeverity(CassLogLevel);
};

}  // namespace bes::dbal::wide::cassandra
