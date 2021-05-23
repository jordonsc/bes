#include "cbt_result.h"

using namespace bes::dbal::wide::bigtable;

CbtResult::CbtResult(cbt::RowReader r) : rows(std::move(r)), cbt_it_end(rows.end()) {}

size_t CbtResult::rowCount()
{
    throw bes::dbal::DbalException("BigTable cannot return a row count");
}

size_t CbtResult::columnCount()
{
    try {
        return (*row_it)->value().cells().size();
    } catch (...) {
        return 0;
    }
}

bes::dbal::wide::Field const& CbtResult::getColumn(size_t n)
{
    throw bes::dbal::DbalException("BigTable cannot return column information");
}

bes::dbal::wide::FieldList const& CbtResult::getColumns()
{
    throw bes::dbal::DbalException("BigTable cannot return column information");
}

void CbtResult::wait()
{
    // Currently no action, the GCP API isn't a future
    // TODO: consider if we want to make our own future pattern here
}

bool CbtResult::pop()
{
    wait();

    if (row_it != nullptr) {
        // First call to pop will generate a new iterator
        row_it = std::make_shared<RowIterator>(rows.begin());
    } else {
        // subsequent calls will increment it
        ++(*row_it);
    }

    if (*row_it == cbt_it_end) {
        return false;
    } else {
        current_row.reset(new CbtRow((*row_it)->value()));
        return true;
    }
}

bes::dbal::wide::Row const* CbtResult::row()
{
    return nullptr;
}
