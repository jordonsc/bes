#pragma once

#include <bes/dbal.h>

#include <memory>

#include "cassandra.h"
#include "cassandra_row.h"
#include "query.tcc"

namespace bes::dbal::wide::cassandra {

class Query;

using WideRow = bes::dbal::wide::Row;
using bes::dbal::wide::ResultInterface;

class CassandraResult : public ResultInterface
{
   public:
    CassandraResult() = delete;
    explicit CassandraResult(Query q);

    [[nodiscard]] size_t rowCount() override;
    [[nodiscard]] size_t columnCount() override;

    bool pop() override;
    [[nodiscard]] WideRow const* row() override;

    [[nodiscard]] Field const& getColumn(size_t n) override;
    [[nodiscard]] FieldList const& getColumns() override;

    void wait() override;

   protected:
    bool advanceRowIterator();

   private:
    Query query;
    bool ready = false;
    FieldList fields;

    std::shared_ptr<CassResult> result;
    std::shared_ptr<CassIterator> row_it;
    std::shared_ptr<CassandraRow> current_row;
    size_t row_count = std::string::npos;
};

}  // namespace bes::dbal::wide::cassandra