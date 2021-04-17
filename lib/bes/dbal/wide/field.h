#pragma once

#include <string>
#include <vector>

#include "datatype.h"

namespace bes::dbal::wide {

struct Field
{
    Field() = default;
    Field(Datatype dt, std::string ns, std::string q) : datatype(dt), ns(std::move(ns)), qualifier(std::move(q)) {}
    Field(std::string ns, std::string q) : datatype(Datatype::Null), ns(std::move(ns)), qualifier(std::move(q)) {}

    Datatype datatype;
    std::string ns;
    std::string qualifier;
};

using FieldList = std::vector<Field>;

}  // namespace bes::dbal::wide
