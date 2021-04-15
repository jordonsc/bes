#include "row_iterator.h"

#include "../../exception.h"

using namespace bes::dbal::wide::cassandra;

RowIterator::RowIterator() : has_data(false) {}

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
        auto const* column = cass_iterator_get_column(row_iterator.get());
        Field f;

        switch (cass_value_type(column)) {
            default:
                throw bes::dbal::DbalException("Unknown or unsupported Cassandra value type");
            case CASS_VALUE_TYPE_ASCII:
            case CASS_VALUE_TYPE_VARCHAR:
            case CASS_VALUE_TYPE_TEXT:
            case CASS_VALUE_TYPE_BLOB:
                f.datatype = Datatype::Text;
                char const* txt;
                size_t s;
                cass_value_get_string(column, &txt, &s);
                cell = std::make_shared<Cell>(Cell(std::move(f), std::make_any<std::string>(std::string(txt, s))));
                break;
            case CASS_VALUE_TYPE_BOOLEAN:
                f.datatype = Datatype::Boolean;
                cass_bool_t b;
                cass_value_get_bool(column, &b);
                cell = std::make_shared<Cell>(Cell(std::move(f), std::make_any<bool>(bool(b))));
                break;
            case CASS_VALUE_TYPE_INT:
                f.datatype = Datatype::Int32;
                int32_t i32;
                cass_value_get_int32(column, &i32);
                cell = std::make_shared<Cell>(Cell(std::move(f), std::make_any<int32_t>(i32)));
                break;
            case CASS_VALUE_TYPE_BIGINT:
                f.datatype = Datatype::Int64;
                int64_t i64;
                cass_value_get_int64(column, &i64);
                cell = std::make_shared<Cell>(Cell(std::move(f), std::make_any<int64_t>(i64)));
                break;
            case CASS_VALUE_TYPE_FLOAT:
                f.datatype = Datatype::Float32;
                float flt;
                cass_value_get_float(column, &flt);
                cell = std::make_shared<Cell>(Cell(std::move(f), std::make_any<float>(flt)));
                break;
            case CASS_VALUE_TYPE_DOUBLE:
            case CASS_VALUE_TYPE_DECIMAL:
                f.datatype = Datatype::Float64;
                double dbl;
                cass_value_get_double(column, &dbl);
                cell = std::make_shared<Cell>(Cell(std::move(f), std::make_any<double>(dbl)));
                break;
        }

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
