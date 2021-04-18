#include "result.h"

using namespace bes::dbal::wide;

Result::Result(FieldList fields) : fields(std::move(fields)) {}

[[nodiscard]] size_t Result::columnCount() const
{
    return fields.size();
};

[[nodiscard]] Field const& Result::getColumn(size_t n) const
{
    try {
        return fields.at(n);
    } catch (std::exception const&) {
        throw bes::dbal::OutOfRangeException("Column " + std::to_string(n) + " out of range");
    }
};

[[nodiscard]] FieldList const& Result::getColumns() const
{
    return fields;
};
