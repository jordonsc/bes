#include "consolelogbackend.h"

#include <unistd.h>

using namespace bes::log::backend;

ConsoleLogBackend::ConsoleLogBackend(LogFormat f, ColourMode use_clr) : fmt(f)
{
    if (use_clr == ColourMode::ENABLE) {
        use_colour = true;
    } else if (use_clr == ColourMode::DISABLE) {
        use_colour = false;
    } else {
        use_colour = isatty(fileno(stdin)) && isatty(fileno(stdout));
    }
}

void ConsoleLogBackend::process(LogRecord const& log_record)
{
    if (isColour()) {
        bool c = true;
        switch (log_record.severity) {
            case bes::log::Severity::TRACE:
            case bes::log::Severity::DEBUG:
                std::cout << BES_CLR_DEBUG;
                break;
            case bes::log::Severity::WARNING:
                std::cout << BES_CLR_WARNING;
                break;
            case bes::log::Severity::ERROR:
                std::cout << BES_CLR_ERROR;
                break;
            case bes::log::Severity::CRITICAL:
            case bes::log::Severity::ALERT:
            case bes::log::Severity::FATAL:
                std::cout << BES_CLR_CRITICAL;
                break;
            default:
                c = false;
                break;
        }

        if (c) {
            log_record.str(std::cout, fmt) << BES_CLR_RESET << std::endl;
        } else {
            log_record.str(std::cout, fmt) << std::endl;
        }

    } else {
        log_record.str(std::cout, fmt) << std::endl;
    }
}

bool ConsoleLogBackend::isColour() const
{
    return use_colour;
}

ConsoleLogBackend& ConsoleLogBackend::setColour(bool v)
{
    use_colour = v;
    return *this;
}
