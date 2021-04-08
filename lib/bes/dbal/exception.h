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

}  // namespace bes::dbal

#endif
