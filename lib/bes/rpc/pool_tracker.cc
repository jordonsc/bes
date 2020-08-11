#include "pool_tracker.h"

using namespace bes::rpc;

PoolTracker::PoolTracker() = default;

void PoolTracker::InstanceSpawning(void* tag)
{
    std::unique_lock<std::mutex> lock(mutex);

    if (inst_map.find(tag) != inst_map.end()) {
        // TODO: consider.. this should never happen, raise warning and return?
        lock.unlock();
        InstanceTerminating(tag);
        lock.lock();
    }

    instances++;
    capacity++;

    inst_map[tag] = InstState::READY;
}

void PoolTracker::InstanceWorking(void* tag)
{
    std::unique_lock<std::mutex> lock(mutex);

    if (inst_map.find(tag) == inst_map.end()) {
        // TODO: raise error, this should never happen
        // Unlock and set up a new instance
        lock.unlock();
        InstanceSpawning(tag);
        lock.lock();
    }

    capacity--;
    inst_map[tag] = InstState::WORKING;
}

void PoolTracker::InstanceTerminating(void* tag)
{
    std::unique_lock<std::mutex> lock(mutex);

    auto search = inst_map.find(tag);
    if (search == inst_map.end()) {
        // TODO: raise error, this should never happen
        // Unlock and run this tag through the motions
        lock.unlock();
        InstanceSpawning(tag);
        InstanceWorking(tag);
        lock.lock();
    } else if (search->second != InstState::WORKING) {
        // It's possible the service was registered then terminated, so we need to drop capacity here as well
        capacity--;
    }

    inst_map.erase(tag);
    instances--;
}

size_t PoolTracker::Count()
{
    return instances;
}

size_t PoolTracker::Capacity()
{
    return capacity;
}
