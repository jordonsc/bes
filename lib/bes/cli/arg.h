#pragma once

#include <memory>
#include <regex>
#include <string>

#include "exception.h"

namespace bes::cli {

class Parser;

enum class ValueType
{
    NONE,       // No value can be specified for this argument
    OPTIONAL,   // A value MAY or may not be specified
    REQUIRED,   // A value MUST be specified IF the option is used
    MANDATORY,  // A value MUST be specified AND the option MUST exist
};

class Arg
{
   public:
    Arg(char s, std::string l, ValueType val_type = ValueType::NONE);
    Arg(char s, std::string l, std::string default_val, ValueType val_type = ValueType::REQUIRED);

    char const short_form;
    std::string const long_form;

    [[nodiscard]] bool present() const;
    [[nodiscard]] std::string const& value() const;
    [[nodiscard]] size_t count() const;
    [[nodiscard]] ValueType argType() const;

    template <class T>
    T as() const;

   protected:
    Arg const operator++(int);
    Arg& operator++();
    Arg& setValue(std::string v);

    size_t counter = 0;
    std::string arg_value;
    bool arg_present = false;
    ValueType value_type;

    friend class ::bes::cli::Parser;
};

template <class T>
T Arg::as() const
{
    throw NoValueException("Unsupported value type");
}

template <>
[[nodiscard]] std::string Arg::as<std::string>() const;

template <>
[[nodiscard]] bool Arg::as<bool>() const;

template <>
[[nodiscard]] int Arg::as<int>() const;

template <>
[[nodiscard]] float Arg::as<float>() const;

}  // namespace bes::cli

