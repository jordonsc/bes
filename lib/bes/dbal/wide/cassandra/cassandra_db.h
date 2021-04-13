#ifndef BES_DBAL_WIDE_CASSANDRA_H
#define BES_DBAL_WIDE_CASSANDRA_H

#include "../../exception.h"
#include "../wide_column_db.h"
#include "connection.h"
#include "keyspace.h"

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

    [[nodiscard]] std::string GetServerVersion() const;
    void SetKeyspace(std::string const&);
    [[nodiscard]] std::string const& GetKeyspace() const;

    void CreateKeyspace(cassandra::Keyspace const& keyspace, bool if_not_exists = false) const;
    void DropKeyspace(std::string const& ks_name, bool if_exists = false) const;

    void CreateTable(std::string const& table_name, Schema const& schema, bool if_not_exists) const override;
    void DropTable(std::string const& table_name, bool if_exists) const override;

   private:
    mutable std::shared_mutex ks_mutex;
    mutable std::string keyspace;

   protected:
    cassandra::Connection connection;

    void ExecuteQuerySync(std::string const& cql) const;

    /**
     * Extracts and error message out of a future.
     */
    static std::string GetFutureErrMsg(CassFuture*);

    /**
     * Return the correct CQL name for a field Datatype.
     *
     * See also: https://cassandra.apache.org/doc/latest/cql/types.html
     */
    [[nodiscard]] static const char* FieldType(Datatype const& dt);

    /**
     * Will return the CQL string for a field.
     */
    [[nodiscard]] static std::string GetFieldCql(Field const& f);

    /**
     * Will raise a NotConnectedException if there is no connection to the server cluster.
     */
    void ValidateConnection() const;
};

}  // namespace bes::dbal::wide

#endif
