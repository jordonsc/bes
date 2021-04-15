#ifndef BES_DBAL_WIDE_CASSANDRA_ROW_ITERATOR_H
#define BES_DBAL_WIDE_CASSANDRA_ROW_ITERATOR_H

#include <iterator>
#include <memory>

#include "../cell.tcc"
#include "cassandra.h"

namespace bes::dbal::wide::cassandra {

class Row;

class RowIterator : public std::iterator<std::input_iterator_tag, Cell>
{
   public:
    using iterator_category = std::input_iterator_tag;
    using value_type = Cell;
    using reference = value_type const&;
    using pointer = std::shared_ptr<Cell const>;
    using difference_type = ptrdiff_t;

    reference operator*() const;
    pointer operator->() const;

    bool operator==(RowIterator const& other) const;
    bool operator!=(RowIterator const& other) const;

    RowIterator const& operator++() const;
    RowIterator const operator++(int) const;

   private:
    explicit RowIterator();
    explicit RowIterator(CassRow const* row);

    mutable bool has_data;
    CassRow const* row;
    mutable std::shared_ptr<CassIterator> row_iterator;
    mutable std::shared_ptr<Cell> cell;

    friend class bes::dbal::wide::cassandra::Row;
};

}  // namespace bes::dbal::wide::cassandra

#endif