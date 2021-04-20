#include "threadpool.h"

using namespace bes;

/**
 * Construct a pool with base capacity of `pool_size`. Capacity may later be increased with `AddThreads()`.
 */
ThreadPool::ThreadPool(threadsize_t pool_size)
{
    if (pool_size <= 0) {
        throw std::runtime_error("Cannot create an empty pool");
    }

    // Build worker functions and threads
    addThreads(pool_size);
}

/**
 * Add new threads to the pool.
 */
void ThreadPool::addThreads(threadsize_t n)
{
    std::unique_lock<std::mutex> lock(queue_mutex);

    worker_count += n;

    for (size_t i = 0; i < n; ++i) {
        workers.emplace_back([this] {
            for (;;) {
                std::function<void()> task;

                {
                    // Wait for work to be added to the queue
                    std::unique_lock<std::mutex> lock(queue_mutex);
                    condition.wait(lock);

                    // Exit if we've no work to do
                    if (stop.load()) {
                        return;
                    }

                    // Grab a task from the front of the queue
                    task = std::move(this->tasks.front());
                    tasks.pop();
                    --backlog_size;
                }

                // Run the task
                task();
            }
        });
    }
}

size_t ThreadPool::threadCount() const
{
    return worker_count.load();
}

size_t ThreadPool::backlog() const
{
    return backlog_size.load();
}

/**
 * Join all threads when destructing.
 */
ThreadPool::~ThreadPool()
{
    stop.store(true);
    condition.notify_all();

    for (auto& t : workers) {
        t.join();
    }
}
