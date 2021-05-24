#pragma once

#include <bes/dbal.h>

#include <memory>

#include "cbt_row.h"
#include "google/cloud/bigtable/table.h"
#include "google/cloud/bigtable/table_admin.h"

namespace bes::dbal::wide {
class BigTable;
}

namespace bes::dbal::wide::bigtable {

namespace cbt = google::cloud::bigtable;
using WideRow = bes::dbal::wide::Row;
using bes::dbal::wide::ResultInterface;
using RowIterator = cbt::RowReader::iterator;

class CbtResult : public ResultInterface
{
   public:
    CbtResult() = delete;
    explicit CbtResult(cbt::RowReader r);

    [[nodiscard]] size_t rowCount() override;
    [[nodiscard]] size_t columnCount() override;
    [[nodiscard]] Field const& getColumn(size_t n) override;
    [[nodiscard]] FieldList const& getColumns() override;

    void wait() override;
    bool pop() override;
    [[nodiscard]] WideRow const* row() override;

   private:
    cbt::RowReader rows;
    std::shared_ptr<RowIterator> row_it;
    std::shared_ptr<CbtRow> current_row;
    RowIterator cbt_it_end;
};

}  // namespace bes::dbal::wide::bigtable
