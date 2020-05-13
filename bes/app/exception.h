#ifndef BES_APP_EXCEPTION_H
#define BES_APP_EXCEPTION_H

#include <stdexcept>

#include "bes/core.h"

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

}  // namespace bes::app

#endif
