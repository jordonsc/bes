#include "utility.h"

using namespace bes::dbal::wide::cassandra;

using bes::dbal::Datatype;
using bes::dbal::wide::Cell;
using bes::dbal::wide::Field;

std::vector<Field> Utility::getColumnsForResult(CassResult const* result)
{
    auto col_count = cass_result_column_count(result);
    std::vector<Field> fields;
    fields.reserve(col_count);

    for (size_t col_idx = 0; col_idx < col_count; ++col_idx) {
        fields.push_back(getFieldFromResult(result, col_idx));
    }

    return fields;
}

Cell Utility::createCellFromColumn(CassValue const* column, Field&& f)
{
    if (column == nullptr) {
        throw bes::dbal::OutOfRangeException("Column out of bounds");
    }

    if (cass_value_is_null(column)) {
        f.datatype = Datatype::Null;
        return Cell(std::move(f), std::make_any<char>(0));
    }

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
            return Cell(std::move(f), std::make_any<std::string>(std::string(txt, s)));
        case CASS_VALUE_TYPE_BOOLEAN:
            f.datatype = Datatype::Boolean;
            cass_bool_t b;
            cass_value_get_bool(column, &b);
            return Cell(std::move(f), std::make_any<bool>(bool(b)));
        case CASS_VALUE_TYPE_INT:
            f.datatype = Datatype::Int32;
            int32_t i32;
            cass_value_get_int32(column, &i32);
            return Cell(std::move(f), std::make_any<int32_t>(i32));
        case CASS_VALUE_TYPE_BIGINT:
            f.datatype = Datatype::Int64;
            int64_t i64;
            cass_value_get_int64(column, &i64);
            return Cell(std::move(f), std::make_any<int64_t>(i64));
        case CASS_VALUE_TYPE_FLOAT:
            f.datatype = Datatype::Float32;
            float flt;
            cass_value_get_float(column, &flt);
            return Cell(std::move(f), std::make_any<float>(flt));
        case CASS_VALUE_TYPE_DOUBLE:
        case CASS_VALUE_TYPE_DECIMAL:
            f.datatype = Datatype::Float64;
            double dbl;
            cass_value_get_double(column, &dbl);
            return Cell(std::move(f), std::make_any<double>(dbl));
    }
}

Field Utility::getFieldFromResult(CassResult const* result, size_t index)
{
    Field f;

    char const* col_name;
    size_t col_name_len;
    cass_result_column_name(result, index, &col_name, &col_name_len);

    // The cass column name isn't null-terminated, which makes strchr() dangerous. Instead we'll convert to a C++
    // string and use the class functions.
    f.qualifier = std::string(col_name, col_name_len);

    switch (cass_result_column_type(result, index)) {
        default:
            throw bes::dbal::DbalException("Unknown or unsupported Cassandra value type");
        case CASS_VALUE_TYPE_ASCII:
        case CASS_VALUE_TYPE_VARCHAR:
        case CASS_VALUE_TYPE_TEXT:
        case CASS_VALUE_TYPE_BLOB:
            f.datatype = Datatype::Text;
            break;
        case CASS_VALUE_TYPE_BOOLEAN:
            f.datatype = Datatype::Boolean;
            break;
        case CASS_VALUE_TYPE_INT:
            f.datatype = Datatype::Int32;
            break;
        case CASS_VALUE_TYPE_BIGINT:
            f.datatype = Datatype::Int64;
            break;
        case CASS_VALUE_TYPE_FLOAT:
            f.datatype = Datatype::Float32;
            break;
        case CASS_VALUE_TYPE_DOUBLE:
        case CASS_VALUE_TYPE_DECIMAL:
            f.datatype = Datatype::Float64;
            break;
    }

    return f;
}
