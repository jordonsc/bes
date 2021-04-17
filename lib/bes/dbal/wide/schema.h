#pragma once

#include <vector>

#include "field.h"

namespace bes::dbal::wide {

struct Schema
{
    Schema(Field pk, FieldList fields) : primary_key(std::move(pk)), fields(std::move(fields)) {}

    Field primary_key;
    FieldList fields;
};

}  // namespace bes::dbal::wide
