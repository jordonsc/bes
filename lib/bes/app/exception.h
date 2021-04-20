#pragma once

#include <bes/core.h>

#include <stdexcept>

namespace bes::app {

class KernelException : public BesException
{
    using BesException::BesException;
};

/**
 * Used for cases that should never occur.
 */
class KernelPanicException : public KernelException
{
    using KernelException::KernelException;
};

class NoDiscoveryInterfaceException : public KernelException
{
    using KernelException::KernelException;
};

}  // namespace bes::app

