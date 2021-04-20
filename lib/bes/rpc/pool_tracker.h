#pragma once

#include <bes/core.h>

#include <cstdint>
#include <iostream>
#include <mutex>
#include <unordered_map>

namespace bes::rpc {

/**
 * Keeps a tally of the state of all workers in a pool
 */
class PoolTracker final
{
   public:
    PoolTracker();

    /**
     * Register a new service as running and accepting connections
     * @param tag
     */
    void instanceSpawning(void* tag);

    /**
     * Instance is now working - it's consuming resources but not accepting capacity
     * @param tag
     */
    void instanceWorking(void* tag);

    /**
     * Resourcing is finishing up, no further resource or capacity is offered by it
     * @param tag
     */
    void instanceTerminating(void* tag);

    size_t count() const;

    size_t capacity() const;

   private:
    enum class InstState
    {
        READY,
        WORKING
    };

    size_t instances = 0;
    size_t max_capacity = 0;
    std::mutex mutex{};
    std::unordered_map<void*, InstState> inst_map{};
};

}  // namespace bes::rpc
