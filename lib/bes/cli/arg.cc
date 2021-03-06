#include "arg.h"

#include <utility>

using namespace bes::cli;

Arg::Arg(char s, std::string l, ValueType val_type) : Arg::Arg(s, std::move(l), "", val_type) {}

Arg::Arg(char s, std::string l, std::string default_val, ValueType val_type)
    : short_form(s), long_form(std::move(l)), arg_value(std::move(default_val)), value_type(val_type)
{
    if (value_type == ValueType::NONE && !arg_value.empty()) {
        throw MalformedArgumentException("Argument cannot have a default value when the value-type is NONE");
    }
}

Arg& Arg::operator++()
{
    BES_LOG(TRACE) << "Arg(--" << long_form << ")++";
    arg_present = true;
    ++counter;

    return *this;
}

Arg const Arg::operator++(int)
{
    Arg tmp = Arg(*this);
    operator++();
    return tmp;
}

Arg& Arg::setValue(std::string v)
{
    if (value_type == ValueType::NONE) {
        throw UnexpectedValueException("Argument --" + long_form + " does not accept a value");
    } else if ((value_type == ValueType::REQUIRED || value_type == ValueType::MANDATORY) && v.length() == 0) {
        throw ValueErrorException("Argument --" + long_form + " must have a value");
    }

    BES_LOG(TRACE) << "Arg(--" << long_form << ") = \"" << v << "\"";
    arg_value = std::move(v);

    return *this;
}

bool Arg::present() const
{
    return arg_present;
}

std::string const& Arg::value() const
{
    return arg_value;
}

size_t Arg::count() const
{
    return counter;
}

ValueType Arg::argType() const
{
    return value_type;
}

template <>
std::string Arg::as<std::string>() const
{
    return arg_value;
}

template <>
bool Arg::as<bool>() const
{
    if (std::regex_match(arg_value, std::regex("(true|yes|1)", std::regex::icase))) {
        return true;
    } else if (std::regex_match(arg_value, std::regex("(false|no|0)", std::regex::icase))) {
        return false;
    }

    throw ValueErrorException("Could not convert value to boolean");
}

template <>
int Arg::as<int>() const
{
    try {
        return std::stoi(arg_value);
    } catch (std::invalid_argument&) {
        throw ValueErrorException("Could not convert value to an integer");
    }
}
template <>
float Arg::as<float>() const
{
    try {
        return std::stof(arg_value);
    } catch (std::invalid_argument&) {
        throw ValueErrorException("Could not convert value to a float");
    }
}
