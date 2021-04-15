#ifndef BES_DBAL_WIDE_CASSANDRA_ROW_H
#define BES_DBAL_WIDE_CASSANDRA_ROW_H

#include <iterator>

#include "cassandra.h"
#include "row_iterator.h"

namespace bes::dbal::wide::cassandra {

class Row
{
   public:
    Row() = delete;
    explicit Row(CassRow const* row);

    [[nodiscard]] RowIterator begin() const;
    [[nodiscard]] RowIterator end() const;

   private:
    CassRow const* row;
};

}  // namespace bes::dbal::wide::cassandra

#endif