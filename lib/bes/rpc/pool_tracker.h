#ifndef BES_RPC_POOL_TRACKER_H
#define BES_RPC_POOL_TRACKER_H

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
    void InstanceSpawning(void* tag);

    /**
     * Instance is now working - it's consuming resources but not accepting capacity
     * @param tag
     */
    void InstanceWorking(void* tag);

    /**
     * Resourcing is finishing up, no further resource or capacity is offered by it
     * @param tag
     */
    void InstanceTerminating(void* tag);

    size_t Count() const;

    size_t Capacity() const;

   private:
    enum class InstState
    {
        READY,
        WORKING
    };

    size_t instances = 0;
    size_t capacity = 0;
    std::mutex mutex{};
    std::unordered_map<void*, InstState> inst_map{};
};

}  // namespace bes::rpc

#endif
