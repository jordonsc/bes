#include "cassandra_row.h"

#include "utility.h"

using namespace bes::dbal::wide::cassandra;

CassandraRow::CassandraRow(CassRow const* r, std::shared_ptr<CassResult> rs) : row(r), result(std::move(rs)) {}

Cell CassandraRow::at(size_t n) const
{
    if (row == nullptr) {
        throw bes::dbal::DbalException("Cannot index null row");
    } else if (result == nullptr) {
        throw bes::dbal::DbalException("Cannot index row: missing result data");
    }

    return Utility::createCellFromColumn(cass_row_get_column(row, n), Utility::getFieldFromResult(result.get(), n));
}

Cell CassandraRow::at(std::string const& qualifier) const
{
    if (row == nullptr) {
        throw DbalException("Cannot index null row");
    }

    Field f(qualifier);

    return cassandra::Utility::createCellFromColumn(cass_row_get_column_by_name(row, qualifier.c_str()), std::move(f));
}
