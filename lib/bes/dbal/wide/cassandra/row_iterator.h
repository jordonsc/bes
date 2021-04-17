#pragma once

#include <iterator>
#include <memory>

#include "../cell.tcc"
#include "cassandra.h"
#include "types.h"
#include "utility.h"

namespace bes::dbal::wide {
template <class IteratorT, class DataT>
class Row;
}

namespace bes::dbal::wide::cassandra {

class RowIterator : public std::iterator<std::input_iterator_tag, Cell>
{
   private:
    explicit RowIterator();
    explicit RowIterator(RowDataType data);

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
    mutable bool has_data;
    CassRow const* row{};
    ResultDataType result;
    mutable size_t pos = 0;
    mutable std::shared_ptr<CassIterator> row_iterator;
    mutable std::shared_ptr<Cell> cell;

    friend class bes::dbal::wide::Row<RowIterator, RowDataType>;
};

}  // namespace bes::dbal::wide::cassandra
