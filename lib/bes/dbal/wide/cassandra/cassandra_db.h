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
    Cassandra() = delete;
    explicit Cassandra(Context c);

    [[nodiscard]] std::string getServerVersion() const;
    void setKeyspace(std::string const&);
    [[nodiscard]] std::string const& getKeyspace() const;

    void createKeyspace(cassandra::Keyspace const& keyspace, bool if_not_exists = false) const;
    void dropKeyspace(std::string const& ks_name, bool if_exists = false) const;

    void createTable(std::string const& table_name, Schema const& schema, bool if_not_exists) const override;
    void dropTable(std::string const& table_name, bool if_exists) const override;

    void update(std::string const& table_name, ValueList values) const override;
    void update(std::string const& table_name, Value const& key, ValueList values) const override;
    void remove(std::string const& table_name, Value const& key) const override;
    void retrieve(const std::string& table_name, const Value& key) const override;

    void createTestData(std::string const& tbl, int a, std::string const& b) const;
    cassandra::ResultT retrieveTestData(std::string const& tbl, int a) const;

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
     * Will return the CQL string for a field or value.
     *
     * Field: namespace_qualifier FIELD_TYPE
     * Value: namespace_qualifier
     */
    [[nodiscard]] static std::string getFieldCql(Field const& f, bool with_field_type = false);
    [[nodiscard]] static std::string getFieldCql(Value const& v, bool with_field_type = false);

    /**
     * Consumes Value and binds it to the Query.
     */
    static void bindValue(cassandra::Query& q, Value v);

    /**
     * Will raise a NotConnectedException if there is no connection to the server cluster.
     */
    void validateConnection() const;
};

}  // namespace bes::dbal::wide
