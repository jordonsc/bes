#ifndef BES_DBAL_WIDE_CASSANDRA_RESULT_ITERATOR_H
#define BES_DBAL_WIDE_CASSANDRA_RESULT_ITERATOR_H

#include <iterator>
#include <memory>

#include "cassandra.h"
#include "row.h"

namespace bes::dbal::wide::cassandra {

class Result;

class ResultIterator : public std::iterator<std::input_iterator_tag, Row>
{
   private:
    ResultIterator();
    explicit ResultIterator(CassResult* r);

   public:
    using iterator_category = std::input_iterator_tag;
    using value_type = Row;
    using reference = value_type const&;
    using pointer = std::shared_ptr<Row const>;
    using difference_type = ptrdiff_t;

    reference operator*() const;
    pointer operator->() const;

    bool operator==(ResultIterator const& other) const;
    bool operator!=(ResultIterator const& other) const;

    ResultIterator const& operator++() const;
    ResultIterator const operator++(int) const;

   private:
    mutable std::shared_ptr<Row> row_ptr;
    CassResult* result;
    std::shared_ptr<CassIterator> result_iterator;
    mutable bool has_data;

    friend class bes::dbal::wide::cassandra::Result;
};

}  // namespace bes::dbal::wide::cassandra

#endif
