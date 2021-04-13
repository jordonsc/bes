#ifndef BES_DBAL_WIDE_SCHEMA_H
#define BES_DBAL_WIDE_SCHEMA_H

#include <vector>

#include "field.h"

namespace bes::dbal::wide {

struct Schema
{
    Field primary_key;
    std::vector<Field> fields;

    Schema(Field&& pk, std::vector<Field>&& fields)
        : primary_key(std::forward<Field>(pk)), fields(std::forward<std::vector<Field>>(fields))
    {}
};

}  // namespace bes::dbal::wide

#endif