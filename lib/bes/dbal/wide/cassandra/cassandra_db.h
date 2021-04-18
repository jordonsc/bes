#pragma once

#include "../../exception.h"
#include "../wide_column_db.h"
#include "connection.h"
#include "keyspace.h"
#include "query.tcc"
#include "types.h"

namespace bes::dbal::wide {

class Cassandra : public WideColumnDb
{
   public:
    Cassandra() = delete;
    explicit Cassandra(Context c);

    [[nodiscard]] std::string getServerVersion() const;
    void setKeyspace(std::string const&);
    [[nodiscard]] std::string const& getKeyspace() const;

    ResultFuture createKeyspace(cassandra::Keyspace const& keyspace, bool if_not_exists = false) const;
    ResultFuture dropKeyspace(std::string const& ks_name, bool if_exists = false) const;

    ResultFuture createTable(std::string const& table_name, Schema const& schema, bool if_not_exists) const override;
    ResultFuture dropTable(std::string const& table_name, bool if_exists) const override;

    ResultFuture insert(std::string const& table_name, ValueList values) const override;
    ResultFuture update(std::string const& table_name, Value const& key, ValueList values) const override;
    ResultFuture retrieve(std::string const& table_name, Value const& key) const override;
    ResultFuture retrieve(std::string const& table_name, Value const& key, FieldList fields) const override;
    ResultFuture remove(std::string const& table_name, Value const& key) const override;
    ResultFuture truncate(std::string const& table_name) const override;

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

    /**
     * Executes a query and generates a ResultFuture object to return.
     */
    ResultFuture execute(cassandra::Query q) const;
};

}  // namespace bes::dbal::wide
