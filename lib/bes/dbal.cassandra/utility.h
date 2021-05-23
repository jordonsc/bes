#pragma once

#include <bes/dbal.h>

#include "cassandra.h"

namespace bes::dbal::wide::cassandra {

using bes::dbal::wide::Cell;
using bes::dbal::wide::Datatype;
using bes::dbal::wide::Field;

class Utility
{
   public:
    static inline std::string getFutureErrMsg(CassFuture* f)
    {
        const char* message;
        size_t message_length;
        cass_future_error_message(f, &message, &message_length);

        return std::string(message, message_length);
    }

    static std::vector<Field> getColumnsForResult(CassResult const* result);
    static Cell createCellFromColumn(CassValue const* column, Field&& f);
    static Field getFieldFromResult(CassResult const* result, size_t index);
};

}  // namespace bes::dbal::wide::cassandra