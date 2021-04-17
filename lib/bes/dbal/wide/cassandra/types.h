#pragma once

#include <memory>

namespace bes::dbal::wide {

template <class IteratorT, class DataT>
class Result;

template <class IteratorT, class DataT>
class Row;

}  // namespace bes::dbal::wide

namespace bes::dbal::wide::cassandra {

static char const NS_DELIMITER = '_';

class ResultIterator;
class RowIterator;

using ResultDataType = std::shared_ptr<CassResult>;
using RowDataType = std::pair<CassRow const*, ResultDataType>;

using ResultT = bes::dbal::wide::Result<ResultIterator, ResultDataType>;
using RowT = bes::dbal::wide::Row<RowIterator, RowDataType>;

}  // namespace bes::dbal::wide::cassandra
