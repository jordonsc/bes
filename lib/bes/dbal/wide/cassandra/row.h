#pragma once

#include "../row.h"
#include "cassandra.h"

namespace bes::dbal::wide::cassandra {

using WideRow = bes::dbal::wide::Row;
using bes::dbal::wide::Cell;
class Result;

class Row : WideRow
{
   public:
    Row() = delete;
    [[nodiscard]] Cell at(size_t) const override;
    [[nodiscard]] Cell at(std::string const& ns, std::string const& qualifier) const override;

   protected:
    explicit Row(CassRow const* r, std::shared_ptr<CassResult>);
    CassRow const* row;
    std::shared_ptr<CassResult> result;

    friend class bes::dbal::wide::cassandra::Result;
};

}  // namespace bes::dbal::wide::cassandra