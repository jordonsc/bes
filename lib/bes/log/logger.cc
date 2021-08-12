#include "logger.h"

using namespace bes::log;

Logger::Logger(Severity severity, char const* function, char const* filename, int lineno)
    : severity(severity), function(function), filename(filename), lineno(lineno)
{
    if (LogSink::hasInstance()) {
        LogSink& s = LogSink::instance();
        log_enabled = !s.empty() && s.enabled(severity);
    } else {
        log_enabled = false;
    }
}

bool Logger::enabled() const
{
    return log_enabled;
}

void Logger::dispatch()
{
    if (!ostream || !log_enabled) {
        return;
    }

    log_enabled = false;

    LogRecord record;
    record.severity = severity;
    record.function = function;
    record.filename = filename;
    record.lineno = lineno;
    record.timestamp = std::chrono::system_clock::now();
    record.message = ostream->str();

    LogSink::instance().log(record);
}

std::ostream& Logger::stream()
{
    if (!ostream) {
        ostream = log_enabled ? std::make_unique<std::ostringstream>() : std::make_unique<NullStream>();
    }

    return *ostream;
}
