#include "result.h"

using namespace bes::dbal::wide::bigtable;

Result::Result(cbt::RowReader r) : rows(std::move(r)) {}

size_t Result::rowCount() const
{
    throw bes::dbal::DbalException("BigTable cannot return a row count");
}

size_t Result::columnCount() const
{
    return 0;
}

bes::dbal::wide::Field const& Result::getColumn(size_t n) const
{
    return Field();
}

bes::dbal::wide::FieldList const& Result::getColumns() const
{
    return FieldList();
}

void Result::wait() const {
    it = rows.begin();
}

bes::dbal::wide::Row const* Result::pop()
{

}

bes::dbal::wide::Row const* Result::row() const
{
    return nullptr;
}
