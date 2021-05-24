#pragma once

#include <memory>
#include <utility>

#include "result_interface.h"

namespace bes::dbal::kv {

/**
 * Wrapper that accepts an implemented ResultInterface object in the form of a shared pointer.
 *
 * The purpose of this class is to create a RAII approach to an abstracted object that would otherwise need to be a
 * pointer.
 */
class ResultFuture
{
   public:
    explicit ResultFuture(std::shared_ptr<bes::dbal::kv::ResultInterface> result) : result(std::move(result)) {}

    /**
     * Wait for query completion. If the query has already completed, return immediately.
     *
     * All other result functions will first call wait to ensure the result is ready.
     */
    inline void wait()
    {
        result->wait();
    }

    inline Text asString()
    {
        return result->getString();
    }

    inline Int64 asInt()
    {
        return result->getInt();
    }

    inline Float64 asFloat()
    {
        return result->getFloat();
    }

    [[nodiscard]] inline ResultInterface* operator->() const
    {
        return result.get();
    }

    [[nodiscard]] inline ResultInterface* get() const
    {
        return result.get();
    }

   protected:
    std::shared_ptr<bes::dbal::kv::ResultInterface> result;
};

}  // namespace bes::dbal::kv