#pragma once

#include <bes/core.h>

namespace bes::dbal {

class DbalException : public bes::BesException
{
    using BesException::BesException;
};

class NotConnectedException : public DbalException
{
    using DbalException::DbalException;
};

class NoRecordException : public DbalException
{
    using DbalException::DbalException;
};

class NullValueException : public DbalException
{
    using DbalException::DbalException;

   public:
    NullValueException() : DbalException("Value is null") {}
    NullValueException(std::string const& a, std::string const& b)
        : DbalException("Value of " + a + ":" + b + " is null")
    {}
};

class OutOfRangeException : public DbalException
{
    using DbalException::DbalException;
};

}  // namespace bes::dbal
