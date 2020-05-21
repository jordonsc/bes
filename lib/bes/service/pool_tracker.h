#ifndef BES_POOL_TRACKER_H
#define BES_POOL_TRACKER_H

#include <iostream>
#include <mutex>
#include <unordered_map>
#include <cstdint>

namespace bes::service {

/**
 * Keeps a tally of the state of all workers in a pool
 */
class PoolTracker final {
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

    size_t Count();

    size_t Capacity();

  private:
    enum class InstState {
        READY,
        WORKING
    };

    size_t instances = 0;
    size_t capacity = 0;
    std::mutex mutex {};
    std::unordered_map<void*, InstState> inst_map {};
};

}   // end namespace

#endif
