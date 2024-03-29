#include "logsink.h"

using namespace bes::log;

LogSink* LogSink::singleton = nullptr;

LogSink::LogSink(Severity s)
{
    if (LogSink::singleton != nullptr) {
        throw std::runtime_error("Cannot recreate a singleton logger");
    }

    severity = static_cast<int>(s);

    LogSink::singleton = this;
}

LogSink::~LogSink()
{
    LogSink::singleton = nullptr;
}

LogSink& LogSink::instance()
{
    return *LogSink::singleton;
}

void LogSink::log(LogRecord const& log_record)
{
    std::shared_lock<std::shared_mutex> lock(backend_mutex);

    for (auto const& backend : backends) {
        backend.second->process(log_record);
    }
}

void LogSink::removeBackend(long const& id)
{
    std::lock_guard<std::shared_mutex> lock(backend_mutex);
    backends.erase(id);
    has_backends.store(!backends.empty());
}

void LogSink::clearBackends()
{
    std::lock_guard<std::shared_mutex> lock(backend_mutex);
    backends.clear();
    has_backends.store(false);
}

std::size_t LogSink::backendCount()
{
    std::shared_lock<std::shared_mutex> lock(backend_mutex);
    return backends.size();
}

/**
 * Return true if this object has no backends.
 */
bool LogSink::empty() const
{
    return !has_backends.load(std::memory_order_relaxed);
}

bool LogSink::enabled(Severity s) const
{
    return severity.load(std::memory_order_relaxed) <= static_cast<int>(s);
}

void LogSink::setSeverity(Severity s)
{
    severity.store(static_cast<int>(s));
}

bool LogSink::hasInstance() noexcept
{
    return LogSink::singleton != nullptr;
}
