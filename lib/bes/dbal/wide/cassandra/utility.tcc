#pragma once

#include "../../exception.h"
#include "../cell.tcc"
#include "cassandra.h"

namespace bes::dbal::wide::cassandra::utility {

using bes::dbal::wide::Cell;
using bes::dbal::wide::Datatype;
using bes::dbal::wide::Field;

inline Cell createCellFromColumn(CassValue const* column)
{
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

}  // namespace bes::dbal::wide::cassandra::utility