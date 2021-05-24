#pragma once

#include <bes/dbal.h>

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

    SuccessFuture createTable(const std::string& table_name, const Schema& schema) override;
    SuccessFuture dropTable(const std::string& table_name) override;
    SuccessFuture apply(const std::string& table_name, const Value& key, ValueList values) override;
    ResultFuture retrieve(const std::string& table_name, const Value& key) override;
    ResultFuture retrieve(const std::string& table_name, const Value& key, FieldList fields) override;
    SuccessFuture remove(const std::string& table_name, const Value& key) override;
    SuccessFuture truncate(const std::string& table_name) override;

   protected:
    bigtable::Credentials credentials;

    google::cloud::bigtable::Table getTable(std::string const& tbl) const;
    google::cloud::bigtable::TableAdmin getTableAdmin() const;

    static std::string getKeyFromValue(Value v);
};

}  // namespace bes::dbal::wide
