#pragma once

#include <bes/dbal.h>

#include "google/cloud/bigtable/table.h"

namespace bes::dbal::wide::bigtable {

namespace cbt = google::cloud::bigtable;
using WideRow = bes::dbal::wide::Row;
using bes::dbal::wide::Cell;
class CbtResult;

class CbtRow : WideRow
{
   public:
    CbtRow() = delete;
    explicit CbtRow(cbt::Row r);

    [[nodiscard]] Cell at(size_t) const override;
    [[nodiscard]] Cell at(std::string const& qualifier) const override;

   protected:
    cbt::Row row;
    static Cell CellFromCbtCell(cbt::Cell c);
};

}  // namespace bes::dbal::wide::bigtable