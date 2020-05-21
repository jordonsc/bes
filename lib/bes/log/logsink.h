#ifndef BES_LOG_LOGSINK_H
#define BES_LOG_LOGSINK_H

#include <atomic>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <sstream>
#include <stdexcept>

#include "model.h"

namespace bes::log {

/**
 * Singleton log sink.
 *
 * This should be constructed by the AppKernel, and should exist for the entirety of the application lifecycle.
 * If an instance does not exist, the Logger class will work around it by forming a NullStream and not dispatching
 * any log records it is asked to build.
 *
 * Log backends may be added or removed at any time, however if there are no backends, the Logger will not dispatch
 * records.
 */
class LogSink
{
   public:
    explicit LogSink(bes::log::Severity s = bes::log::Severity::NOTICE);
    ~LogSink();

    LogSink(LogSink&) = delete;
    LogSink& operator=(LogSink&) = delete;

    static LogSink& Instance();
    static bool HasInstance() noexcept;

    void Log(LogRecord const& log_record);

    template <class T, class... Args>
    long AddBackend(Args&&... args);

    void RemoveBackend(long const& id);
    void ClearBackends();
    std::size_t BackendCount();

    void SetSeverity(bes::log::Severity);

    /**
     * Return true if this object has no backends.
     */
    [[nodiscard]] bool Empty() const;

    /**
     * Return false if we're not logging at this severity
     */
    [[nodiscard]] bool Enabled(Severity s) const;

   private:
    std::shared_mutex backend_mutex;
    std::atomic<bool> has_backends{false};
    std::map<long, std::shared_ptr<bes::log::backend::LogBackend>> backends;
    std::atomic<int> severity{0};
    static LogSink* singleton;
};

template <class T, class... Args>
long LogSink::AddBackend(Args&&... args)
{
    std::lock_guard<std::shared_mutex> lock(backend_mutex);
    static long id = 0;
    backends[id] = std::make_shared<T>(std::forward<Args>(args)...);
    has_backends.store(true);
    return id++;
}

}  // namespace bes::log

#endif
