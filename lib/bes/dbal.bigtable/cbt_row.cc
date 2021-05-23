#include "cbt_row.h"

using namespace bes::dbal::wide::bigtable;

CbtRow::CbtRow(cbt::Row r) : row(std::move(r)) {}

Cell CbtRow::at(size_t n) const
{
    try {
        return CellFromCbtCell(row.cells().at(n));
    } catch (std::out_of_range const&) {
        throw bes::dbal::OutOfRangeException("No cell at index " + std::to_string(n));
    }
}

Cell CbtRow::at(std::string const& qualifier) const
{
    // Will need to do a scan of the vector, no known other approaches at this stage..
    for (auto const& c : row.cells()) {
        // TODO: need to consider c.family_name() here, too
        if (c.column_qualifier() == qualifier) {
            return CellFromCbtCell(c);
        }
    }

    throw bes::dbal::OutOfRangeException("No cell for index '" + qualifier + "'");
}

/**
 * Convert a BigTable cell to a Bes cell.
 */
Cell CbtRow::CellFromCbtCell(cbt::Cell c)
{
    // TODO: consider: auto f = Field(c.family_name() + "_" + c.column_qualifier());
    auto f = Field(c.column_qualifier());
    return Cell(std::move(f), std::any(std::move(c).value()));
}
