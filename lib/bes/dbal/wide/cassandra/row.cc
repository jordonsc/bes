#include "row.h"

using namespace bes::dbal::wide::cassandra;

Row::Row(CassRow const* r) : row(r) {}

RowIterator Row::begin() const
{
    return RowIterator(row);
}

RowIterator Row::end() const
{
    return RowIterator();
}
