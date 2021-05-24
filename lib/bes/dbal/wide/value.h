#pragma once

#include <any>
#include <string>
#include <vector>

#include "../datatype.h"

namespace bes::dbal::wide {

class Value final
{
   public:
    // -- Standard Value Types -- //
    Value(std::string qualifier, std::string value)
        : qualifier(std::move(qualifier)),
          value(std::move(value)),
          datatype(Datatype::Text)
    {}

    Value(std::string qualifier, char const* value)
        : qualifier(std::move(qualifier)),
          value(std::string(value)),
          datatype(Datatype::Text)
    {}

    Value(std::string qualifier, Int32 value) : qualifier(std::move(qualifier)), value(value), datatype(Datatype::Int32)
    {}

    Value(std::string qualifier, Int64 value) : qualifier(std::move(qualifier)), value(value), datatype(Datatype::Int64)
    {}

    Value(std::string qualifier, Float32 value)
        : qualifier(std::move(qualifier)),
          value(value),
          datatype(Datatype::Float32)
    {}

    Value(std::string qualifier, Float64 value)
        : qualifier(std::move(qualifier)),
          value(value),
          datatype(Datatype::Float64)
    {}

    Value(std::string qualifier, Boolean value)
        : qualifier(std::move(qualifier)),
          value(value),
          datatype(Datatype::Boolean)
    {}

    Value(std::string qualifier) : qualifier(std::move(qualifier)), value((char)0), datatype(Datatype::Null) {}

    Value() : value((char)0), datatype(Datatype::Null) {}

    // -- List Value Types (nb: no bool or null) -- //
    Value(std::string qualifier, std::vector<std::string> value)
        : qualifier(std::move(qualifier)),
          is_list(true),
          value(std::move(value)),
          datatype(Datatype::Text)
    {}

    Value(std::string qualifier, std::vector<Int32> value)
        : qualifier(std::move(qualifier)),
          is_list(true),
          list_size(value.size()),
          value(std::move(value)),
          datatype(Datatype::Int32)
    {}

    Value(std::string qualifier, std::vector<Int64> value)
        : qualifier(std::move(qualifier)),
          is_list(true),
          list_size(value.size()),
          value(std::move(value)),
          datatype(Datatype::Int64)
    {}

    Value(std::string qualifier, std::vector<Float32> value)
        : qualifier(std::move(qualifier)),
          is_list(true),
          list_size(value.size()),
          value(std::move(value)),
          datatype(Datatype::Float32)
    {}

    Value(std::string qualifier, std::vector<Float64> value)
        : qualifier(std::move(qualifier)),
          is_list(true),
          list_size(value.size()),
          value(std::move(value)),
          datatype(Datatype::Float64)
    {}

    [[nodiscard]] std::string const& getQualifier() const
    {
        return qualifier;
    }

    [[nodiscard]] std::any const& getValue() const&
    {
        return value;
    }

    std::any&& getValue() &&
    {
        return std::move(value);
    }

    std::any&& consumeValue()
    {
        return std::move(value);
    }

    [[nodiscard]] Datatype getDatatype() const
    {
        return datatype;
    }

    [[nodiscard]] bool isList() const
    {
        return is_list;
    }

    [[nodiscard]] size_t size() const
    {
        return list_size;
    }

   private:
    std::string qualifier;
    bool is_list = false;
    size_t list_size = 0;
    std::any value;
    Datatype datatype;
};

using ValueList = std::vector<Value>;

}  // namespace bes::dbal::wide
