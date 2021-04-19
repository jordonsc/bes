#include "result_iterator.h"

#include <utility>

using namespace bes::dbal::wide;

ResultIterator::ResultIterator() : has_data(false) {}

ResultIterator::ResultIterator(std::shared_ptr<bes::dbal::wide::Result> result)
    : has_data(true), result(std::move(result))
{
    this->operator++();
}

ResultIterator::reference ResultIterator::operator*() const
{
    return *row;
}

ResultIterator::pointer ResultIterator::operator->() const
{
    return &*row;
}

bool ResultIterator::operator==(ResultIterator const& other) const
{
    return has_data == other.has_data;
}

bool ResultIterator::operator!=(ResultIterator const& other) const
{
    return !(*this == other);
}

ResultIterator const& ResultIterator::operator++() const
{
    auto row_ptr = result->pop();

    if (row_ptr) {
        row = std::make_shared<RowContainer>(RowContainer(row_ptr));
    } else {
        has_data = false;
        row.reset();
    }

    return *this;
}

ResultIterator const ResultIterator::operator++(int) const
{
    auto tmp = *this;
    ++(*this);
    return tmp;
}
