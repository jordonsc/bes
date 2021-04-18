#pragma once

#include <memory>

#include "../result.h"
#include "row.h"
#include "cassandra.h"

namespace bes::dbal::wide::cassandra {

class Query;

using WideRow = bes::dbal::wide::Row;
using WideResult = bes::dbal::wide::Result;

class Result : public WideResult
{
   public:
    [[nodiscard]] size_t rowCount() const override;
    WideRow const* pop() override;
    [[nodiscard]] WideRow const* row() override;

   private:
    Result(std::shared_ptr<CassResult> r, FieldList fields);

    std::shared_ptr<CassResult> result;
    std::shared_ptr<CassIterator> row_it;
    std::shared_ptr<Row> row_ptr;
    mutable size_t row_count = std::string::npos;

    friend class bes::dbal::wide::cassandra::Query;
};

}  // namespace bes::dbal::wide::cassandra