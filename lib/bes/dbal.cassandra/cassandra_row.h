#pragma once

#include <bes/dbal.h>

#include "cassandra.h"

namespace bes::dbal::wide::cassandra {

using WideRow = bes::dbal::wide::Row;
using bes::dbal::wide::Cell;
class CassandraResult;

class CassandraRow : WideRow
{
   public:
    CassandraRow() = delete;
    [[nodiscard]] Cell at(size_t) const override;
    [[nodiscard]] Cell at(std::string const& qualifier) const override;

   protected:
    explicit CassandraRow(CassRow const* r, std::shared_ptr<CassResult>);
    CassRow const* row;
    std::shared_ptr<CassResult> result;

    friend class bes::dbal::wide::cassandra::CassandraResult;
};

}  // namespace bes::dbal::wide::cassandra