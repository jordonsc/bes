#include "result.h"

using namespace bes::dbal::wide::cassandra;

Result::Result(Query q) : query(std::move(q)) {}

size_t Result::rowCount() const
{
    wait();

    // Lazy-load the row count
    if (row_count == std::string::npos) {
        row_count = cass_result_row_count(result.get());
    }

    return row_count;
}

WideRow const* Result::pop()
{
    wait();

    if (cass_iterator_next(row_it.get())) {
        CassRow const* cass_row = cass_iterator_get_row(row_it.get());
        row_ptr.reset(new Row(cass_row, result));
    } else {
        row_ptr.reset();
        return nullptr;
    }

    return row_ptr.get();
}

WideRow const* Result::row() const
{
    wait();
    return row_ptr.get();
}

[[nodiscard]] size_t Result::columnCount() const
{
    wait();
    return fields.size();
}

[[nodiscard]] bes::dbal::wide::Field const& Result::getColumn(size_t n) const
{
    wait();

    try {
        return fields.at(n);
    } catch (std::exception const&) {
        throw bes::dbal::OutOfRangeException("Column " + std::to_string(n) + " out of range");
    }
}

[[nodiscard]] bes::dbal::wide::FieldList const& Result::getColumns() const
{
    wait();
    return fields;
}

void Result::wait() const
{
    if (ready) {
        return;
    }

    ready = true;

    auto cass_result = query.getResult();
    result = std::shared_ptr<CassResult>(cass_result,
                                         [](CassResult* item) {
                                             cass_result_free(item);
                                         }),

    fields = Utility::getColumnsForResult(cass_result);

    row_it = std::shared_ptr<CassIterator>(cass_iterator_from_result(result.get()), [](CassIterator* i) {
        cass_iterator_free(i);
    });
}
