#ifndef BES_FASTCGI_EXCEPTION_H
#define BES_FASTCGI_EXCEPTION_H

#include <bes/core.h>

namespace bes::fastcgi {

class FastCgiException : public bes::BesException
{
    using BesException::BesException;
};

class IndexErrorException : public FastCgiException
{
    using FastCgiException::FastCgiException;
};

class PayloadException : public FastCgiException
{
    using FastCgiException::FastCgiException;
};

class AbortException : public FastCgiException
{
    using FastCgiException::FastCgiException;
};

}  // namespace bes::fastcgi

#endif
