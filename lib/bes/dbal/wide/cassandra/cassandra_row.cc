#include "cassandra_row.h"

#include "types.h"
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

Cell CassandraRow::at(std::string const& ns, std::string const& qualifier) const
{
    if (row == nullptr) {
        throw DbalException("Cannot index null row");
    }

    Field f(ns, qualifier);

    std::string fqn;
    if (!ns.empty()) {
        fqn.append(ns);
        fqn += cassandra::NS_DELIMITER;
    }
    fqn.append(qualifier);

    return cassandra::Utility::createCellFromColumn(cass_row_get_column_by_name(row, fqn.c_str()), std::move(f));
}
