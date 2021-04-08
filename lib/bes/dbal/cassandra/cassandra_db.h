#ifndef BES_DBAL_CASSANDRA_H
#define BES_DBAL_CASSANDRA_H

#include "../exception.h"
#include "../wide.h"
#include "connection.h"

namespace bes::dbal {

class Cassandra : public WideColumnDb
{
   public:
    explicit Cassandra(cassandra::Connection&&);
    explicit Cassandra(std::string);

    std::string GetServerVersion();

   protected:
    cassandra::Connection connection;

    /**
     * Will raise a NotConnectedException if there is no connection to the server cluster.
     */
    void ValidateConnection();
};

}  // namespace bes::dbal

#endif
