#pragma once

#include <bes/core.h>

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
