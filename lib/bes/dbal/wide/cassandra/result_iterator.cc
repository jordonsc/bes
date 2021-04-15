#include "result_iterator.h"

#include "../../exception.h"

using namespace bes::dbal::wide::cassandra;

ResultIterator::ResultIterator() : result(nullptr), has_data(false){};

ResultIterator::ResultIterator(CassResult* r) : result(r), has_data(true)
{
    result_iterator.reset(cass_iterator_from_result(result), [](CassIterator* i) {
        cass_iterator_free(i);
    });

    ++*this;
}

ResultIterator::reference ResultIterator::operator*() const
{
    if (row_ptr == nullptr) {
        throw bes::dbal::DbalException("Cannot provide a reference to a null row");
    }

    return *row_ptr;
}

ResultIterator::pointer ResultIterator::operator->() const
{
    if (row_ptr == nullptr) {
        throw bes::dbal::DbalException("Cannot provide a pointer to a null row");
    }

    return row_ptr;
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
    if (!has_data) {
        throw bes::dbal::DbalException("Iterator exhausted");
    } else if (!result_iterator) {
        throw bes::dbal::DbalException("ResultIterator attempted operation with a null child iterator");
    }

    has_data = cass_iterator_next(result_iterator.get());

    if (has_data) {
        row_ptr = std::make_shared<Row>(cass_iterator_get_row(result_iterator.get()));
    } else {
        row_ptr.reset();
    }

    return *this;
}

ResultIterator const ResultIterator::operator++(int) const
{
    ResultIterator tmp(*this);
    ++*this;
    return tmp;
}
