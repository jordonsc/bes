#include "model.h"

std::ostream &operator<<(std::ostream &stream, bes::log::Severity const &s)
{
    switch (s) {
        case bes::log::Severity::TRACE:
            stream << "TRACE";
            break;
        case bes::log::Severity::DEBUG:
            stream << "DEBUG";
            break;
        case bes::log::Severity::INFO:
            stream << "INFO";
            break;
        case bes::log::Severity::NOTICE:
            stream << "NOTICE";
            break;
        case bes::log::Severity::WARNING:
            stream << "WARNING";
            break;
        case bes::log::Severity::ERROR:
            stream << "ERROR";
            break;
        case bes::log::Severity::CRITICAL:
            stream << "CRITICAL";
            break;
        case bes::log::Severity::ALERT:
            stream << "ALERT";
            break;
        case bes::log::Severity::FATAL:
            stream << "FATAL";
            break;
        default:
            throw std::runtime_error("Undefined log severity");
    }

    return stream;
}

std::ostream &bes::log::LogRecord::str(std::ostream &stream, bes::log::LogFormat f) const
{
    switch (f) {
        case bes::log::LogFormat::MSG_ONLY:
            stream << message;
            break;
        case bes::log::LogFormat::SHORT:
            stream << "[" << severity << "] " << message;
            break;
        default:
        case bes::log::LogFormat::STANDARD:
            fmt_tp(stream) << " " << message;
            break;
        case bes::log::LogFormat::FULL:
            fmt_tp(stream) << " [" << severity << "] " << message;
            break;
        case bes::log::LogFormat::DETAIL:
            fmt_tp(stream) << " [" << severity << "] " << message << " [" << filename << ": " << lineno << " ("
                           << function << ")]";
            break;
    }

    return stream;
}

/**
 * Formatter for our time-point.
 *
 * NB: We have thread safety issues due to the std::localtime invocation, this function should be rebuilt with the
 *     availability of C++20 and it's improved chrono libraries.
 *
 * https://en.cppreference.com/w/cpp/io/manip/put_time
 */
std::ostream &bes::log::LogRecord::fmt_tp(std::ostream &stream) const
{
    std::time_t time = std::chrono::system_clock::to_time_t(timestamp);

    // FIXME: std::localtime is not thread safe
    std::tm time_tm = *std::localtime(&time);

    stream << std::put_time(&time_tm, "%F %T");

    return stream;
}
