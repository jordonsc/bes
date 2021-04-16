#pragma once

#include "../../exception.h"
#include "../wide_column_db.h"
#include "connection.h"
#include "keyspace.h"
#include "query.tcc"
#include "types.h"

namespace bes::dbal::wide {

namespace cassandra {
static char const* const KEYSPACE_PARAM = "keyspace";
}

class Cassandra : public WideColumnDb
{
   public:
    explicit Cassandra(cassandra::Connection&&);
    explicit Cassandra(std::string hosts);
    Cassandra(cassandra::Connection&&, Context&& c);
    Cassandra(std::string hosts, Context&& c);
    Cassandra(cassandra::Connection&&, Context const& c);
    Cassandra(std::string hosts, Context const& c);

    [[nodiscard]] std::string getServerVersion() const;
    void setKeyspace(std::string const&);
    [[nodiscard]] std::string const& getKeyspace() const;

    void createKeyspace(cassandra::Keyspace const& keyspace, bool if_not_exists = false) const;
    void dropKeyspace(std::string const& ks_name, bool if_exists = false) const;

    void createTable(std::string const& table_name, Schema const& schema, bool if_not_exists) const override;
    void dropTable(std::string const& table_name, bool if_exists) const override;

    void createTestData(std::string const& tbl, int a, std::string const& b);
    cassandra::ResultT retrieveTestData(std::string const& tbl, int a);

   private:
    mutable std::shared_mutex ks_mutex;
    mutable std::string keyspace;

   protected:
    cassandra::Connection connection;

    /**
     * Return the correct CQL name for a field Datatype.
     *
     * See also: https://cassandra.apache.org/doc/latest/cql/types.html
     */
    [[nodiscard]] static const char* fieldType(Datatype const& dt);

    /**
     * Will return the CQL string for a field.
     */
    [[nodiscard]] static std::string getFieldCql(Field const& f);

    /**
     * Will raise a NotConnectedException if there is no connection to the server cluster.
     */
    void validateConnection() const;
};

}  // namespace bes::dbal::wide
