#pragma once

#include <any>
#include <string>
#include <vector>

#include "datatype.h"

namespace bes::dbal::wide {

struct Value
{
    Value(std::string ns, std::string qualifier, std::string value)
        : ns(std::move(ns)), qualifier(std::move(qualifier)), value(std::move(value)), datatype(Datatype::Text)
    {}

    Value(std::string ns, std::string qualifier, char const* value)
        : ns(std::move(ns)), qualifier(std::move(qualifier)), value(std::string(value)), datatype(Datatype::Text)
    {}

    Value(std::string ns, std::string qualifier, Int32 value)
        : ns(std::move(ns)), qualifier(std::move(qualifier)), value(value), datatype(Datatype::Int32)
    {}

    Value(std::string ns, std::string qualifier, Int64 value)
        : ns(std::move(ns)), qualifier(std::move(qualifier)), value(value), datatype(Datatype::Int64)
    {}

    Value(std::string ns, std::string qualifier, Float32 value)
        : ns(std::move(ns)), qualifier(std::move(qualifier)), value(value), datatype(Datatype::Float32)
    {}

    Value(std::string ns, std::string qualifier, Float64 value)
        : ns(std::move(ns)), qualifier(std::move(qualifier)), value(value), datatype(Datatype::Float64)
    {}

    Value(std::string ns, std::string qualifier, Boolean value)
        : ns(std::move(ns)), qualifier(std::move(qualifier)), value(value), datatype(Datatype::Boolean)
    {}

    Value(std::string ns, std::string qualifier)
        : ns(std::move(ns)), qualifier(std::move(qualifier)), value((char)0), datatype(Datatype::Null)
    {}

    Value() : value((char)0), datatype(Datatype::Null) {}

    std::string ns;
    std::string qualifier;
    std::any value;
    Datatype datatype;
};

using ValueList = std::vector<Value>;



}  // namespace bes::dbal::wide
