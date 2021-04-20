#ifndef BES_LOG_MODEL_H
#define BES_LOG_MODEL_H

#include <chrono>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <sstream>
#include <stdexcept>

namespace bes::log {

/**
 * Log message alert level.
 */
enum class Severity : int
{
    // Use this level for messages that indicate the code is entering and leaving functions
    TRACE,

    // Use this level for debug messages that should not be present in production
    DEBUG,

    // Informational messages, such as normal progress
    INFO,

    // Informational messages, such as unusual, but expected conditions
    NOTICE,

    // An indication of problems, users may need to take action
    WARNING,

    // An error has been detected, action _is_ required
    ERROR,

    // The system is in a critical state, such as running out of local resources
    CRITICAL,

    // The system is at risk of immediate failure
    ALERT,

    // The system is about to crash or terminate
    FATAL,

    // Valid boundaries
    //    MIN = Severity::TRACE,
    //    MAX = Severity::FATAL,
};

/**
 * Quick format selection for text backends.
 */
enum class LogFormat
{
    // MSG
    MSG_ONLY,

    // [SEV] MSG
    SHORT,

    // TIME MSG
    STANDARD,

    // TIME [SEV] MSG
    FULL,

    // TIME [SEV] MSG [FILE: LINE (FUNC)]
    DETAIL,
};

/**
 * Represents a single log message.
 */
class LogRecord
{
   public:
    Severity severity;
    std::string function;
    std::string filename;
    int lineno;
    std::chrono::system_clock::time_point timestamp;
    std::string message;

    ::std::ostream &str(::std::ostream &stream, LogFormat f) const;

   private:
    ::std::ostream &fmt_tp(::std::ostream &stream) const;
};

namespace backend {
/**
 * Base class for all log backends.
 */
class LogBackend
{
   public:
    virtual ~LogBackend() = default;

    virtual void process(LogRecord const &log_record) = 0;
};
}  // namespace backend

struct NullStream : ::std::ostringstream
{
    /// Generic do-nothing streaming operator
    template <typename T>
    NullStream &operator<<(T)
    {
        return *this;
    }
};

}  // namespace bes::log

// Output Severity as a human-readable string
std::ostream &operator<<(std::ostream &stream, bes::log::Severity const &s);

/**
 * Create a log entry with a fully-qualified log level.
 */
#define BES_LOG_LVL(level)                                                                                    \
    for (auto BES_LOG_INST = ::bes::log::Logger(level, __func__, __FILE__, __LINE__); BES_LOG_INST.enabled(); \
         BES_LOG_INST.dispatch())                                                                             \
    BES_LOG_INST.stream()

/**
 * Primary entry-point for logging, allows short-hand severity:
 *      BES_LOG(INFO) << "Test Log";
 */
#define BES_LOG(level) BES_LOG_LVL(::bes::log::Severity::level)

#endif
