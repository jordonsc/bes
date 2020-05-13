#ifndef BES_NET_EXCEPTION_H
#define BES_NET_EXCEPTION_H

#include "lib/bes/bes.h"

namespace bes::net {

class NetException : public bes::BesException
{
    using BesException::BesException;
};

class SocketException : public NetException
{
    using NetException::NetException;
};

class SocketBindException : public SocketException
{
    using SocketException::SocketException;
};

}  // namespace bes::net

#endif
