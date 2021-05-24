#pragma once

#include <functional>
#include <utility>

namespace bes::dbal::kv {

/**
 * Determines if a result was successful or failed, in the form of a future.
 *
 * Construct an instance by passing a lambda that should close your future and return a result.
 *
 * If the task returns a positive or negative result, then the return value should reflect this.
 * If the task has a hard error, then it MUST throw an exception on error instead of returning false.
 */
class SuccessFuture final
{
   public:
    using FnSig = std::function<bool()>;

    SuccessFuture() = delete;
    explicit SuccessFuture(FnSig f) : lambda(std::move(f)) {}
    explicit SuccessFuture(bool b)
        : lambda([b]() {
              return b;
          })
    {}

    /**
     * Waits for the task to complete, ignoring result.
     *
     * Will throw an exception on error.
     */
    inline void wait()
    {
        lambda();
    }

    /**
     * Waits for task to complete and returns a boolean success status.
     *
     * Will throw an exception on error.
     */
    [[nodiscard]] inline bool ok()
    {
        return lambda();
    }

   private:
    FnSig lambda;
};

}  // namespace bes::dbal::kv