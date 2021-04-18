#include "result.h"

using namespace bes::dbal::wide::cassandra;

Result::Result(std::shared_ptr<CassResult> r, bes::dbal::wide::FieldList fields)
    : WideResult(std::move(fields)), result(std::move(r))
{
    row_it = std::shared_ptr<CassIterator>(cass_iterator_from_result(result.get()), [](CassIterator* i) {
        cass_iterator_free(i);
    });
}

size_t Result::rowCount() const
{
    // Lazy-load the row count
    if (row_count == std::string::npos) {
        row_count = cass_result_row_count(result.get());
    }

    return row_count;
}

WideRow const* Result::pop()
{
    if (cass_iterator_next(row_it.get())) {
        CassRow const* cass_row = cass_iterator_get_row(row_it.get());
        row_ptr.reset(new Row(cass_row, result));
    } else {
        row_ptr.reset();
        return nullptr;
    }

    return row_ptr.get();
}

WideRow const* Result::row()
{
    return row_ptr.get();
}