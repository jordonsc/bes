#include "logger.h"

using namespace bes::log;

Logger::Logger(Severity severity, char const* function, char const* filename, int lineno)
    : severity(severity), function(function), filename(filename), lineno(lineno)
{
    if (LogSink::HasInstance()) {
        LogSink& s = LogSink::Instance();
        log_enabled = !s.Empty() && s.Enabled(severity);
    } else {
        log_enabled = false;
    }
}

bool Logger::Enabled() const
{
    return log_enabled;
}

void Logger::Dispatch()
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

    LogSink::Instance().Log(std::move(record));
}

std::ostream& Logger::Stream()
{
    if (!ostream) {
        ostream = log_enabled ? std::make_unique<std::ostringstream>() : std::make_unique<NullStream>();
    }

    return *ostream;
}
