#include "lib/log/logsink.h"

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

LogSink& LogSink::Instance()
{
    return *LogSink::singleton;
}

void LogSink::Log(LogRecord log_record)
{
    std::shared_lock<std::shared_mutex> lock(backend_mutex);

    for (auto backend : backends) {
        backend.second.get()->Process(log_record);
    }
}

void LogSink::RemoveBackend(long const& id)
{
    std::lock_guard<std::shared_mutex> lock(backend_mutex);
    backends.erase(id);
    has_backends.store(backends.size() > 0);
}

void LogSink::ClearBackends()
{
    std::lock_guard<std::shared_mutex> lock(backend_mutex);
    backends.clear();
    has_backends.store(false);
}

std::size_t LogSink::BackendCount()
{
    std::shared_lock<std::shared_mutex> lock(backend_mutex);
    return backends.size();
}

/**
 * Return true if this object has no backends.
 */
bool LogSink::Empty() const
{
    return !has_backends.load(std::memory_order_relaxed);
}

bool LogSink::Enabled(Severity s) const
{
    return severity.load(std::memory_order_relaxed) <= static_cast<int>(s);
}

void LogSink::SetSeverity(Severity s)
{
    severity.store(static_cast<int>(s));
}

bool LogSink::HasInstance() noexcept
{
    return LogSink::singleton != nullptr;
}
