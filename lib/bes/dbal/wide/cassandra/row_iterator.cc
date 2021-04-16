#include "row_iterator.h"

#include "../../exception.h"

using namespace bes::dbal::wide::cassandra;

RowIterator::RowIterator() : has_data(false), row(nullptr) {}

RowIterator::RowIterator(CassRow const* r) : has_data(true), row(r)
{
    row_iterator.reset(cass_iterator_from_row(row), [](CassIterator* item) {
        cass_iterator_free(item);
    });

    ++*this;
}

RowIterator::value_type const& RowIterator::operator*() const
{
    return *cell;
}

RowIterator::pointer RowIterator::operator->() const
{
    return cell;
}

bool RowIterator::operator==(RowIterator const& other) const
{
    return has_data == other.has_data;
}

bool RowIterator::operator!=(RowIterator const& other) const
{
    return !(*this == other);
}

RowIterator const& RowIterator::operator++() const
{
    if (!has_data) {
        throw bes::dbal::DbalException("Cassandra row iterator exhausted");
    } else if (!row_iterator) {
        throw bes::dbal::DbalException("Cassandra row iterator attempted operation with a null child iterator");
    }

    has_data = cass_iterator_next(row_iterator.get());

    if (has_data) {
        cell = std::make_shared<Cell>(Utility::createCellFromColumn(cass_iterator_get_column(row_iterator.get())));
    } else {
        cell.reset();
    }

    return *this;
}

RowIterator const RowIterator::operator++(int) const
{
    RowIterator tmp(*this);
    ++*this;
    return tmp;
}
