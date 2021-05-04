#pragma once

#include "../wide_column_db.h"
#include "credentials.h"
#include "google/cloud/bigtable/table.h"
#include "google/cloud/bigtable/table_admin.h"

namespace bes::dbal::wide {

/**
 * Consider: https://cloud.google.com/bigtable/docs/schema-design
 */
class BigTable : public WideColumnDb
{
   public:
    explicit BigTable(Context c);

    ResultFuture createTable(const std::string& table_name, const Schema& schema, bool if_not_exists) const override;
    ResultFuture dropTable(const std::string& table_name, bool if_exists) const override;
    ResultFuture insert(const std::string& table_name, ValueList values) const override;
    ResultFuture update(const std::string& table_name, const Value& key, ValueList values) const override;
    ResultFuture retrieve(const std::string& table_name, const Value& key) const override;
    ResultFuture retrieve(const std::string& table_name, const Value& key, FieldList fields) const override;
    ResultFuture remove(const std::string& table_name, const Value& key) const override;
    ResultFuture truncate(const std::string& table_name) const override;

   protected:
    bigtable::Credentials credentials;

    google::cloud::bigtable::Table getTable(std::string const& tbl) const;
    google::cloud::bigtable::TableAdmin getTableAdmin() const;

    static std::string getKeyFromValue(Value v);
};

}  // namespace bes::dbal::wide
