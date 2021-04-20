#include "application.h"

using namespace bes::app;

void Application::configureCli(bes::cli::Parser& parser)
{
    using bes::cli::Arg;
    using bes::cli::ValueType;

    // Standard options
    parser << Arg('v', "verbose") << Arg('c', "config", ValueType::REQUIRED) << Arg(0, "version") << Arg(0, "help");
}

void Application::configureLogger(bes::log::LogSink& log_sink, bes::log::Severity verbosity)
{
    using bes::log::LogFormat;
    using bes::log::Severity;

    // Add a console backend as default operation
    log_sink.addBackend<bes::log::backend::ConsoleLogBackend>(verbosity <= Severity::TRACE ? LogFormat::DETAIL
                                                                                           : LogFormat::STANDARD);
}

bool Application::kernelExists()
{
    return KernelInterface::exists();
}

KernelInterface& Application::kernel()
{
    return KernelInterface::getInstance();
}

std::string const& Application::getUsage()
{
    if (usage.length() == 0 && kernelExists()) {
        std::stringstream ss;
        ss << "Usage:\n  " << getKey();

        size_t num_args = kernel().getCli().argCount();
        size_t mandatory = 0;
        auto args = kernel().getCli().getAllArgs();

        // We'll put all the mandatory arguments on the main usage line:
        for (auto const* a : args) {
            if (a->argType() == bes::cli::ValueType::MANDATORY) {
                ++mandatory;
                if (a->short_form) {
                    ss << " -" << a->short_form << "|";
                } else {
                    ss << " ";
                }
                ss << "--" << a->long_form << "=<value>";
            }
        }

        // If there are non-mandatory arguments remaining, list them as options
        if (mandatory < num_args) {
            ss << " [options]\n\nOptions:\n";
            for (auto const* a : args) {
                if (a->argType() == bes::cli::ValueType::MANDATORY) {
                    continue;
                }

                if (a->short_form) {
                    ss << "  -" << a->short_form;
                } else {
                    ss << "    ";
                }
                ss << "  --" << a->long_form;

                switch (a->argType()) {
                    default:
                    case bes::cli::ValueType::NONE:
                        ss << "\n";
                        break;
                    case bes::cli::ValueType::OPTIONAL:
                        ss << " [value]\n";
                        break;
                    case bes::cli::ValueType::REQUIRED:
                        ss << " <value>\n";
                        break;
                }
            }
        } else {
            ss << "\n";
        }

        usage = ss.str();
    }

    return usage;
}
