#pragma once

#include "../row.h"

namespace bes::dbal::wide::bigtable {

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

    friend class bes::dbal::wide::bigtable::Result;
};

}  // namespace bes::dbal::wide::cassandra