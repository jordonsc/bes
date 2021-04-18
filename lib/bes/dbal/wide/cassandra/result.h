#pragma once

#include <memory>

#include "../result.h"
#include "cassandra.h"
#include "query.tcc"
#include "row.h"

namespace bes::dbal::wide::cassandra {

class Query;

using WideRow = bes::dbal::wide::Row;
using WideResult = bes::dbal::wide::Result;

class Result final : public WideResult
{
   public:
    Result() = delete;
    explicit Result(Query q);

    [[nodiscard]] size_t rowCount() const override;
    [[nodiscard]] size_t columnCount() const override;

    WideRow const* pop() override;
    [[nodiscard]] WideRow const* row() const override;

    [[nodiscard]] Field const& getColumn(size_t n) const override;
    [[nodiscard]] FieldList const& getColumns() const override;

    void wait() const override;

   private:
    mutable Query query;
    mutable bool ready = false;
    mutable FieldList fields;

    mutable std::shared_ptr<CassResult> result;
    mutable std::shared_ptr<CassIterator> row_it;
    mutable std::shared_ptr<Row> row_ptr;
    mutable size_t row_count = std::string::npos;

    friend class bes::dbal::wide::cassandra::Query;
};

}  // namespace bes::dbal::wide::cassandra