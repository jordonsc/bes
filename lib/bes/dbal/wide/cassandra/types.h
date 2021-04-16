#pragma once

#include <memory>

namespace bes::dbal::wide {

template <class IteratorT, class DataT>
class Result;

template <class IteratorT, class DataT>
class Row;

}  // namespace bes::dbal::wide

namespace bes::dbal::wide::cassandra {

class ResultIterator;
class RowIterator;

using ResultT = bes::dbal::wide::Result<ResultIterator, std::shared_ptr<CassResult>>;
using RowT = bes::dbal::wide::Row<RowIterator, CassRow const*>;

}  // namespace bes::dbal::wide::cassandra
