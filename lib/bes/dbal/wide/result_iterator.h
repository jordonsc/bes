#pragma once

#include <iterator>

#include "result.h"
#include "row_container.h"

namespace bes::dbal::wide {

class ResultFuture;

class ResultIterator : public std::iterator<std::input_iterator_tag, RowContainer>
{
   public:
    using iterator_category = std::input_iterator_tag;
    using value_type = RowContainer;
    using reference = value_type const&;
    using pointer = value_type const*;
    using difference_type = ptrdiff_t;

    reference operator*() const;
    pointer operator->() const;

    bool operator==(ResultIterator const& other) const;
    bool operator!=(ResultIterator const& other) const;

    ResultIterator const& operator++() const;
    ResultIterator const operator++(int) const;

   protected:
    ResultIterator();
    explicit ResultIterator(std::shared_ptr<bes::dbal::wide::Result> result);

    mutable bool has_data;
    std::shared_ptr<bes::dbal::wide::Result> result;
    mutable std::shared_ptr<bes::dbal::wide::RowContainer> row;

    friend class bes::dbal::wide::ResultFuture;
};

}  // namespace bes::dbal::wide