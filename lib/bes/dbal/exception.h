#ifndef BES_DBAL_EXCEPTION_H
#define BES_DBAL_EXCEPTION_H

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
};

class OutOfRangeException : public DbalException
{
    using DbalException::DbalException;
};

}  // namespace bes::dbal

#endif
