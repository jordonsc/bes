#ifndef BES_LOG_LOGGER_H
#define BES_LOG_LOGGER_H

#include <chrono>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <sstream>
#include <stdexcept>

#include "logsink.h"
#include "model.h"

namespace bes::log {

/**
 * The Logger class is a bridge between the pre-processor macros and the LogSink.
 *
 * It is responsible:
 * - validating if we can log,
 * - building the LogRecord, and finally
 * - dispatching it to the sink
 */
class Logger
{
   public:
    Logger(Severity severity, char const* function, char const* filename, int lineno);

    [[nodiscard]] bool Enabled() const;

    /// Send the log record captured by this object to @p sink.
    void Dispatch();

    /// Return the iostream that captures the log message.
    std::ostream& Stream();

   private:
    bool log_enabled;
    Severity severity;
    char const* function;
    char const* filename;
    int lineno;
    std::unique_ptr<std::ostringstream> ostream;
};

}  // namespace bes::log

#endif