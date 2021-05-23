#include "cassandra_result.h"

using namespace bes::dbal::wide::cassandra;

CassandraResult::CassandraResult(Query q) : query(std::move(q)) {}

size_t CassandraResult::rowCount()
{
    wait();

    // Lazy-load the row count
    if (row_count == std::string::npos) {
        row_count = cass_result_row_count(result.get());
    }

    return row_count;
}

bool CassandraResult::pop()
{
    wait();
    return advanceRowIterator();
}

WideRow const* CassandraResult::row()
{
    return current_row.get();
}

[[nodiscard]] size_t CassandraResult::columnCount()
{
    wait();
    return fields.size();
}

[[nodiscard]] bes::dbal::wide::Field const& CassandraResult::getColumn(size_t n)
{
    wait();

    try {
        return fields.at(n);
    } catch (std::exception const&) {
        throw bes::dbal::OutOfRangeException("Column " + std::to_string(n) + " out of range");
    }
}

[[nodiscard]] bes::dbal::wide::FieldList const& CassandraResult::getColumns()
{
    wait();
    return fields;
}

void CassandraResult::wait()
{
    if (ready) {
        return;
    }

    ready = true;

    auto cass_result = query.getResult();
    result = std::shared_ptr<CassResult>(
        cass_result,
        [](CassResult* item) {
            cass_result_free(item);
        }),

    fields = Utility::getColumnsForResult(cass_result);

    row_it = std::shared_ptr<CassIterator>(cass_iterator_from_result(result.get()), [](CassIterator* i) {
        cass_iterator_free(i);
    });
}

/**
 * Advances the Cassandra row iterator.
 *
 * Does _NOT_ call wait().
 */
bool CassandraResult::advanceRowIterator()
{
    if (cass_iterator_next(row_it.get())) {
        CassRow const* cass_row = cass_iterator_get_row(row_it.get());
        current_row.reset(new CassandraRow(cass_row, result));
        return true;
    } else {
        current_row.reset();
        return false;
    }
}
