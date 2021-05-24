#pragma once

#include <bes/dbal.h>

#include "connection.h"
#include "constants.h"
#include "keyspace.h"
#include "query.tcc"

namespace bes::dbal::wide {

class Cassandra : public WideColumnDb
{
   public:
    Cassandra() = delete;
    explicit Cassandra(Context c);

    [[nodiscard]] std::string getServerVersion();
    void setKeyspace(std::string const&);
    [[nodiscard]] std::string const& getKeyspace() const;

    SuccessFuture createKeyspace(cassandra::Keyspace const& keyspace, bool if_not_exists = false);
    SuccessFuture dropKeyspace(std::string const& ks_name, bool if_exists = false);

    SuccessFuture createTable(std::string const& table_name, Schema const& schema) override;
    SuccessFuture dropTable(std::string const& table_name) override;

    SuccessFuture apply(std::string const& table_name, Value const& key, ValueList values) override;
    ResultFuture retrieve(std::string const& table_name, Value const& key) override;
    ResultFuture retrieve(std::string const& table_name, Value const& key, FieldList fields) override;
    SuccessFuture remove(std::string const& table_name, Value const& key) override;
    SuccessFuture truncate(std::string const& table_name) override;

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
     * Will raise an exception if the value is a list value
     */
    static void validateNotList(Value const&);

    /**
     * Add a WHERE clause to the CQL string
     */
    static void appendWhereClause(std::string& cql, const Value& key, bool final = true);

    /**
     * Executes a query and generates a ResultFuture object to return.
     */
    ResultFuture execute(cassandra::Query q) const;

    /**
     * Executes a query and generates a SuccessFuture object (with no result).
     */
    SuccessFuture executeSuccess(cassandra::Query q) const;
};

}  // namespace bes::dbal::wide
