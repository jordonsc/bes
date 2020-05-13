#ifndef BES_APP_APPLICATION_H
#define BES_APP_APPLICATION_H

#include <string>
#include <tuple>

#include "kernel.h"
#include "bes/cli.h"

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
    virtual inline std::string const& Name() const
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
    virtual inline std::string const& Key() const
    {
        return key;
    }

    /**
     * A single-line human-friendly description of this application.
     *
     * Eg: "Regularly sends me an email telling me I'm awesome."
     */
    virtual inline std::string const& Description() const
    {
        return description;
    }

    /**
     * Application version in sem-ver format.
     *
     * Eg. (1, 6, 9)
     */
    virtual inline version_t const& Version() const
    {
        return version;
    }

    /**
     * Printed to the console when there is a CLI error or requested by the end-user.
     *
     * Will be automatically generated if you set usage to a blank string.
     */
    virtual std::string const& Usage();

    /**
     * Allow the Application to add or override the CLI arguments.
     *
     * By default, we add standard options such as --config and --verbosity.
     */
    virtual void ConfigureCli(bes::cli::Parser& parser);

    /**
     * Allow the Application to add backends or otherwise configure the log sink.
     *
     * By default, we'll add a ConsoleLogBackend with the format set to STANDARD unless we're in TRACE level verbosity,
     * whereby the log format is set to DETAIL.
     */
    virtual void ConfigureLogger(bes::log::LogSink& log_sink, bes::log::Severity verbosity);

    /**
     * The Application's core process.
     *
     * This is run when the kernel has finished initialisation.
     */
    virtual void Run() = 0;

    /**
     * A request to terminate has been made, close listeners and immediately prepare to shutdown.
     *
     * This function should block until the application has completed shutdown.
     */
    virtual void Shutdown(){};

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
        : name(name), key(key), description(description), version(version), usage(usage)
    {}

    static bool KernelExists();
    static KernelInterface& Kernel();

   private:
    std::string const name;
    std::string const key;
    std::string const description;
    bes::version_t const version;
    std::string usage;
};

}  // namespace bes::app

#endif
