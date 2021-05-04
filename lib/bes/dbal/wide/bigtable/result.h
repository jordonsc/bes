#pragma once

#include "../result.h"
#include "google/cloud/bigtable/table.h"
#include "google/cloud/bigtable/table_admin.h"

namespace cbt = google::cloud::bigtable;

namespace bes::dbal::wide {
class BigTable;
}

namespace bes::dbal::wide::bigtable {

using WideRow = bes::dbal::wide::Row;
using WideResult = bes::dbal::wide::Result;
decltype(cbt::RowReader.begin()) RowIterator;

class Result : public WideResult
{
   public:
    Result() = delete;
    explicit Result(cbt::RowReader r);

    [[nodiscard]] size_t rowCount() const override;
    [[nodiscard]] size_t columnCount() const override;
    [[nodiscard]] Field const& getColumn(size_t n) const override;
    [[nodiscard]] FieldList const& getColumns() const override;
    void wait() const override;
    WideRow const* pop() override;
    [[nodiscard]] WideRow const* row() const override;

   private:
    cbt::RowReader const rows;
    RowIterator it;
};

}  // namespace bes::dbal::wide::bigtable
