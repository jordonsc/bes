#pragma once

#include <string>
#include <utility>

#include "../datatype.h"

namespace bes::dbal::kv {

/**
 * Abstract result object (should be a future) that should be extended by the DBAL implementation.
 */
class ResultInterface
{
   public:
    /**
     * Wait for query completion. If the query has already completed, return immediately.
     *
     * All other result functions will first call wait to ensure the result is ready.
     */
    virtual void wait() = 0;

    virtual Text getString() = 0;
    virtual Int64 getInt() = 0;
    virtual Float64 getFloat() = 0;
};

}  // namespace bes::dbal::kv
