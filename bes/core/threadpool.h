#ifndef BES_CONCURRENCY_THREADPOOL_H
#define BES_CONCURRENCY_THREADPOOL_H

#include <cassert>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <vector>

namespace bes{

using threadsize_t = std::uint16_t;

class ThreadPool
{
   public:
    explicit ThreadPool(threadsize_t pool_size = 10);

    template <class F, class... Args>
    auto Enqueue(F&& f, Args&&... args) -> std::future<typename std::invoke_result<F, Args...>::type>;
    void AddThreads(threadsize_t n);

    /**
     * Size of our pool, active or idle.
     */
    [[nodiscard]] size_t ThreadCount() const;

    /**
     * Number of tasks waiting for a thread to become available.
     */
    [[nodiscard]] size_t Backlog() const;

    ~ThreadPool();

   protected:
    // Worker thread pool
    std::vector<std::thread> workers;

    std::atomic<unsigned> worker_count{0};
    std::atomic<unsigned> backlog_size{0};

    // Task queue
    std::queue<std::function<void()>> tasks;

    // Locks both the tasks and workers lists for read/write
    std::mutex queue_mutex;

    // Thread semaphores
    std::condition_variable condition;
    std::atomic<bool> stop{false};
};

/**
 * Add a task to be processed by the thread pool.
 */
template <class F, class... Args>
auto ThreadPool::Enqueue(F&& f, Args&&... args) -> std::future<typename std::invoke_result<F, Args...>::type>
{
    using return_type = typename std::invoke_result<F, Args...>::type;

    auto task =
        std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    std::future<return_type> res = task->get_future();
    {
        // don't allow enqueueing after stopping the pool
        if (stop.load()) {
            throw std::runtime_error("Enqueue on stopped ThreadPool");
        }

        std::unique_lock<std::mutex> lock(queue_mutex);
        ++backlog_size;
        tasks.emplace([task]() {
            (*task)();
        });
    }

    condition.notify_one();
    return res;
}

}  // namespace bes::concurrency

#endif
