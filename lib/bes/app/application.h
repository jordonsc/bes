#pragma once

#include <bes/cli.h>

#include <string>
#include <tuple>
#include <utility>

#include "kernel.h"

namespace bes::app {

/**
 * The Application class is the foundation of any service. This class contains all your business logic and allows you to
 * customise the application.
 *
 * In here, you'll outline what RPCs you want, customise kernel resources such as the logger or CLI and well as
 * defining any further application logic that isn't bundled into the framework.
 */
class Application
{
   public:
    /**
     * Human-friendly application name, in title-case.
     *
     * Eg: "My Application"
     */
    [[nodiscard]] virtual inline std::string const& getName() const
    {
        return name;
    }

    /**
     * A short application ID, used for things such as identifying to other services and filenames.
     *
     * As a recommended convention, keep it to alphanumeric characters with hyphens/underscores only and only in
     * lowercase.
     *
     * Eg: "my-application"
     */
    [[nodiscard]] virtual inline std::string const& getKey() const
    {
        return key;
    }

    /**
     * A single-line human-friendly description of this application.
     */
    [[nodiscard]] virtual inline std::string const& getDescription() const
    {
        return description;
    }

    /**
     * Application version in sem-ver format.
     *
     * Eg. (1, 6, 9)
     */
    [[nodiscard]] virtual inline version_t const& getVersion() const
    {
        return version;
    }

    /**
     * Printed to the console when there is a CLI error or requested by the end-user.
     *
     * Will be automatically generated if you set usage to a blank string.
     */
    virtual std::string const& getUsage();

    /**
     * Allow the Application to add or override the CLI arguments.
     *
     * By default, we add standard options such as --config and --verbosity.
     */
    virtual void configureCli(bes::cli::Parser& parser);

    /**
     * Allow the Application to add backends or otherwise configure the log sink.
     *
     * By default, we'll add a ConsoleLogBackend with the format set to STANDARD unless we're in TRACE level verbosity,
     * whereby the log format is set to DETAIL.
     */
    virtual void configureLogger(bes::log::LogSink& log_sink, bes::log::Severity verbosity);

    /**
     * The Application's core process.
     *
     * This is run when the kernel has finished initialisation.
     */
    virtual void run() = 0;

    /**
     * A request to terminate has been made, close listeners and immediately prepare to shutdown.
     *
     * This function should block until the application has completed shutdown.
     */
    virtual void shutdown(){};

   protected:
    /**
     * This is a standard constructor that will set some constants for Application metadata.
     *
     * This will be called before the kernel has completed initialisation, so don't use it to perform any logic that
     * requires the kernel resources such as the CLI or configuration containers.
     *
     * You don't need to use it, but doing so allows you to skip implementing a handful of small functions:
     *  - Name()
     *  - Key()
     *  - Description()
     *  - Version()
     *  - Usage()
     */
    Application(std::string name, std::string key, std::string description, bes::version_t version,
                std::string usage = "")
        : name(std::move(name)),
          key(std::move(key)),
          description(std::move(description)),
          version(std::move(version)),
          usage(std::move(usage))
    {}

    static bool kernelExists();
    static KernelInterface& kernel();

   private:
    std::string const name;
    std::string const key;
    std::string const description;
    bes::version_t const version;
    std::string usage;
};

}  // namespace bes::app

