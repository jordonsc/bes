#ifndef BES_DBAL_CASSANDRA_CONNECTION_H
#define BES_DBAL_CASSANDRA_CONNECTION_H

#include <memory>
#include <string>

#include "cassandra.h"

namespace bes::dbal::cassandra {

class Connection
{
   public:
    explicit Connection(std::string hosts);

    [[nodiscard]] std::shared_ptr<CassSession> GetSession() const;
    [[nodiscard]] CassSession* GetSessionPtr() const;
    [[nodiscard]] bool IsConnected() const;

   protected:
    std::string hosts;
    std::shared_ptr<CassFuture> connect_future;
    std::shared_ptr<CassCluster> cluster;
    std::shared_ptr<CassSession> session;
    bool connected = false;
};

}  // namespace bes::dbal::cassandra

#endif