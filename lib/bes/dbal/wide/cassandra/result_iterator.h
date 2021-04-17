#pragma once

#include <iterator>
#include <memory>

#include "../row.tcc"
#include "cassandra.h"
#include "types.h"

namespace bes::dbal::wide::cassandra {

class ResultIterator : public std::iterator<std::input_iterator_tag, RowT>
{
   private:
    ResultIterator();
    explicit ResultIterator(std::shared_ptr<CassResult> r);

   public:
    using iterator_category = std::input_iterator_tag;
    using value_type = RowT;
    using reference = value_type const&;
    using pointer = std::shared_ptr<RowT const>;
    using difference_type = ptrdiff_t;

    reference operator*() const;
    pointer operator->() const;

    bool operator==(ResultIterator const& other) const;
    bool operator!=(ResultIterator const& other) const;

    ResultIterator const& operator++() const;
    ResultIterator const operator++(int) const;

   private:
    mutable std::shared_ptr<RowT> row_ptr;
    std::shared_ptr<CassResult> result;
    std::shared_ptr<CassIterator> result_iterator;
    mutable bool has_data;

    friend class bes::dbal::wide::Result<ResultIterator, ResultDataType>;
};

}  // namespace bes::dbal::wide::cassandra
