#pragma once

#include <string>
#include <vector>

#include "../datatype.h"

namespace bes::dbal::wide {

struct Field
{
    Field() = default;
    Field(Datatype dt, std::string q) : datatype(dt), qualifier(std::move(q)) {}
    explicit Field(std::string q) : datatype(Datatype::Null), qualifier(std::move(q)) {}
    explicit Field(char const* q) : datatype(Datatype::Null), qualifier(std::string(q)) {}

    Datatype datatype = Datatype::Null;
    std::string qualifier;
};

using FieldList = std::vector<Field>;

}  // namespace bes::dbal::wide
