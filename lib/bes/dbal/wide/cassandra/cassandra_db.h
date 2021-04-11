#ifndef BES_DBAL_WIDE_CASSANDRA_H
#define BES_DBAL_WIDE_CASSANDRA_H

#include "../../exception.h"
#include "../wide_column_db.h"
#include "connection.h"

namespace bes::dbal::wide {

class Cassandra : public WideColumnDb
{
   public:
    explicit Cassandra(cassandra::Connection&&);
    explicit Cassandra(std::string);

    std::string GetServerVersion();

    void CreateTable(std::string table_name, Schema schema, bool if_not_exists) override;
    void DropTable(std::string table_name, bool if_exists) override;

   protected:
    cassandra::Connection connection;

    void ExecuteQuerySync(std::string const& cql);

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
    void ValidateConnection();
};

}  // namespace bes::dbal::wide

#endif
