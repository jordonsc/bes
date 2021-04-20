#pragma once

#include <bes/cli.h>
#include <bes/core.h>
#include <bes/log.h>
#include <bes/net.h>

#include <chrono>
#include <condition_variable>
#include <csignal>
#include <cstdlib>
#include <memory>
#include <shared_mutex>

#include "discovery/sidecar.h"
#include "exception.h"

namespace bes::app {

using di_t = ::bes::app::discovery::DiscoveryInterface;

class Application;

/**
 * A kernel should maintain some core services and run an Application.
 *
 * A LogSink, CLI Parser & Config are core services made available from the kernel. The Application or services can call
 * upon them as required.
 *
 * You're unlikely to want to inherit from this interface directly, but rather build an implementation of the `Kernel`
 * class template.
 */
class KernelInterface
{
   public:
    KernelInterface(int argc, char** argv);
    virtual ~KernelInterface();
    KernelInterface(KernelInterface&) = delete;
    KernelInterface& operator=(KernelInterface&) = delete;

    /**
     * Run the kernel initialisation phase.
     *
     * This should bootstrap resources required before running the primary execution loop. If this returns non-zero,
     * application execution should halt and the program exit with the returned value
     */
    virtual int init() = 0;

    /**
     * Run the application.
     *
     * Following a bootstrap process, this will process whatever the binary is supposed to achieve and return an exit
     * code for the operating system. The application is expected to terminate when this function returns and exit with
     * the return value.
     */
    virtual int run() = 0;

    /**
     * Return the correct usage for the application.
     */
    virtual std::string usage() = 0;

    bes::log::LogSink& getLogSink() const;
    bes::cli::Parser const& getCli() const;
    bes::Config const& getConfig();
    bes::Container& getContainer();

    static KernelInterface& getInstance();
    static bool exists();

   protected:
    static KernelInterface* instance;

    int const argc;
    char** const argv;

    std::unique_ptr<bes::log::LogSink> log_sink;
    bes::cli::Parser cli_parser;
    bes::Config config;
    bes::Container container;
};

/**
 * The Kernel template is your Application-aware implementation.
 *
 * The implementation will build the Application, allow it to configure the CLI and then finally run it in an
 * error-controlled environment.
 */
template <class AppT>
class Kernel : public KernelInterface
{
   public:
    template <class... AppArgs>
    Kernel(int argc, char** argv, AppArgs&&... args);
    Kernel(Kernel&) = delete;
    Kernel& operator=(Kernel&) = delete;
    ~Kernel() override;

    /**
     * Executes the initialisation, custom builder and run phase.
     *
     * If any stage returns non-zero, it will return that value and cease further processing. This function is intended
     * to be a single-liner for the `int main()` function.
     *
     * If the argument is not a nullptr, it will be executed between the Init() and Run() calls. This is the ideal place
     * to put a discovery interface builder which would require the kernel's container to be built.
     */
    int Execute(std::function<int()> const& builder = nullptr);

    /**
     * Configure the CLI, config, etc.
     */
    int init() override;

    /**
     * Runs the application in an error-controlled environment.
     */
    int run() override;

    /**
     * Return application CLI usage
     */
    std::string usage() override;

    /**
     * Request the entire kernel and application shutdown and exit.
     */
    void Shutdown();

    /**
     * Check if we've had a termination request.
     *
     * Returns with the signal that was used to request a shutdown, or zero if no shutdown has been requested.
     */
    int ExitRequestStatus();

    /**
     * Block indefinitely until a shutdown request is made.
     */
    void WaitForExit();

    /**
     * Wait until a shutdown request is made or given `time` has expired.
     *
     * Returns true if a shutdown request was made, else false.
     */
    template <class R>
    bool WaitForExit(std::chrono::duration<R> const& time);

   private:
    std::unique_ptr<AppT> app;
    bool has_inited = false;

    sigset_t signal_intercept{};
    std::atomic<int> exit_request{0};
    std::mutex exit_cv_mutex;
    std::condition_variable exit_cv;

    std::thread signal_thread;

    /**
     * Spawns a thread to catch signals.
     *
     * Will send a condition_variable notify_all when a signal has been received. You can use ExitRequestStatus() to
     * check the signal sent.
     */
    void InitSignalInterception();

    /**
     * Run code in this function to control the way the application terminates.
     *
     * If `f` completes without error, an EXIT_SUCCESS code will be returned and nothing logged or printed to stderr. If
     * the code throws an exception, depending on the exception the output will be managed. The application should
     * always return (int main()'s return value) with the value returned by this function.
     *
     * To control the exit code without logging any output, use a SilentExitException or a ManagedExitException.
     */
    int ErrorControlledExecution(std::function<int()> const& f);

    /**
     * This will respond to CLI arguments such as --help, --version, etc.
     */
    int ExecuteKernelCli();

    /// Load the DI container with some core data and services
    void WaitForDI();

    /// Builds the logger
    void InitLogger(size_t verbosity = 0);

    /// Runs the config parser, if a valid file exists
    void InitConfig();

    /// Load the DI container with some core data and services
    void LoadContainer();
};

template <class AppT>
template <class... AppArgs>
Kernel<AppT>::Kernel(int const argc, char** const argv, AppArgs&&... args) : KernelInterface(argc, argv)
{
    // Build the app
    app = std::make_unique<AppT>(std::forward<AppArgs>(args)...);
}

template <class AppT>
int Kernel<AppT>::init()
{
    if (has_inited) {
        throw std::runtime_error("Application attempting to re-init the kernel");
    }
    has_inited = true;

    // From here on in, we'll run in an exception handler
    return ErrorControlledExecution([this] {
        app.get()->configureCli(cli_parser);
        cli_parser.parse(argc, argv);

        try {
            // Init the logger according to the verbosity in the CLI
            InitLogger(cli_parser["verbose"].count());
        } catch (bes::cli::NoSuchArgumentException&) {
            // App removed the standard --verbose option, cannot alter default verbosity
            InitLogger();
        }

        BES_LOG(INFO) << "Init <" << app.get()->getKey() << ">";

        // Load the configuration file, will consider the --config option from the CLI if present
        InitConfig();

        // Bootstrap the service container with some base values & services
        LoadContainer();

        // Allow the kernel to respond to CLI commands (such as --help or --version) before starting the app
        int cli_result = ExecuteKernelCli();
        if (cli_result != static_cast<int>(ExitCode::SUCCESS)) {
            return cli_result;
        }

        // Block signals from here-on in
        InitSignalInterception();

        return static_cast<int>(ExitCode::SUCCESS);
    });
}

template <class AppT>
int Kernel<AppT>::run()
{
    if (!has_inited) {
        throw std::runtime_error("Attempted to run application before initialising kernel");
    }

    return ErrorControlledExecution([this] {
        // If the application didn't create a DiscoveryInterface before executing Run(), we'll create a standard Sidecar
        // now. If this model isn't appropriate, consider creating one in a lambda function passed to Exec().
        if (!di_t::hasDiscoveryInterface()) {
            BES_LOG(INFO) << "Building a sidecar as the default service-discovery mechanic";
            di_t::setDiscoveryInterface<bes::app::discovery::Sidecar>(config);
        }

        // Wait for the DiscoveryInterface to come online
        WaitForDI();

        /**
         * The Application may now start its main processing. We expect the app to start a thread for a process loop and
         * return almost immediately.
         */
        app.get()->run();

        // Wait for shutdown
        // The only way to pass this point is to send a SIGINT or SIGTERM (as Kernel::Shutdown() does)
        BES_LOG(NOTICE) << "Service <" << app.get()->getKey() << "> (" << app.get()->getName() << ") online";
        WaitForExit();

        // Allow additional signals to kill us if we're taking too long
        pthread_sigmask(SIG_UNBLOCK, &signal_intercept, nullptr);

        BES_LOG(TRACE) << "Beginning shutdown sequence";
        // Control the order things shutdown
        app.get()->shutdown();
        di_t::getDiscoveryInterface()->shutdown();

        return signalToExitCode(ExitRequestStatus());
    });
}

/**
 * Prepare our signal interception, running in a suspended thread.
 *
 * This will fire off a notify_all with the `exit_cv` condition variable. This thread is suspended via sigwait(),
 * allowing it to still be fully POSIX compliant.
 */
template <class AppT>
void Kernel<AppT>::InitSignalInterception()
{
    sigemptyset(&signal_intercept);

    // SIGINT for ctrl+c on the keyboard, SIGTERM for graceful shutdown
    sigaddset(&signal_intercept, SIGINT);
    sigaddset(&signal_intercept, SIGTERM);
    sigaddset(&signal_intercept, SIGUSR1);

    // Block the signals, we'll do a quick graceful shutdown when either are received
    pthread_sigmask(SIG_BLOCK, &signal_intercept, nullptr);

    signal_thread = std::thread([&, this]() {
        // The thread will sit and wait here until a signal is delivered
        int signum = 0;
        sigwait(&signal_intercept, &signum);

        // Send log item
        if (signum == SIGINT) {
            BES_LOG(NOTICE) << "Intercept received, shutting down..";
        } else if (signum == SIGTERM) {
            BES_LOG(NOTICE) << "Terminate request received, shutting down..";
        } else if (signum == SIGUSR1) {
            // This is used to indicate we want to exit this loop without further action
            exit_request.store(SIGUSR1);
            return;
        } else {
            BES_LOG(WARNING) << "Unexpected signal intercepted: " << signum << ", shutting down..";
        }

        /**
         * We'll use the signal we received as our "exit has been requested" flag.
         *
         * It's important this value is non-zero, as a zero value indicates no signal was sent and implies this thread
         * is still active. `signum` should never be zero, but if it is, assign it a false value.
         */
        exit_request.store(signum > 0 ? signum : SIGUSR1);

        // Notify anyone waiting for an exit flag
        exit_cv.notify_all();
    });
}

template <class AppT>
inline void Kernel<AppT>::InitLogger(size_t verbosity)
{
    using bes::log::LogFormat;
    using bes::log::Severity;

    if (bes::log::LogSink::hasInstance()) {
        BES_LOG(WARNING) << "Logger already initialised, skipping";
        return;
    }

    // We'll set the min-severity according to the verbosity, with WARNING as our baseline
    Severity sev = static_cast<Severity>(std::max<int>(0, static_cast<int>(Severity::WARNING) - verbosity));

    // Build a log sink; it will be available as a singleton so long as this pointer still has a reference
    log_sink = std::make_unique<bes::log::LogSink>(sev);

    // Hand over to the application to add any further backends or changes
    app.get()->configureLogger(*(log_sink.get()), sev);

    BES_LOG(TRACE) << "Logger ready";
}

template <class AppT>
void Kernel<AppT>::InitConfig()
{
    try {
        try {
            // We'll check the CLI --config option first
            std::string cfg = cli_parser["config"].value();
            if (cfg.length() == 0) {
                throw bes::cli::NoSuchArgumentException(std::string{});
            }

            // When we load with an explicit config file, a FileNotFoundException if there is an issue reading it
            config.loadFile(cfg);

        } catch (bes::cli::NoSuchArgumentException&) {
            // Load using a file finder
            bes::FileFinder ff;
            ff.AppendSearchPath(app.get()->getKey().c_str() + std::string(".yaml"));

            // This will log a warning if it can't find anything, but otherwise carry on
            config.loadFile(ff);
        }
    } catch (bes::FileNotFoundException& e) {
        BES_LOG(FATAL) << e.message();
        throw ManagedExitException("Could not read from specified configuration file", ExitCode::CONFIG_ERR);
    }
}

template <class AppT>
int Kernel<AppT>::ExecuteKernelCli()
{
    try {
        if (cli_parser["help"].present()) {
            std::cout << app.get()->usage();
            return static_cast<int>(ExitCode::CLI_SUCCESS);
        }
    } catch (bes::cli::NoSuchArgumentException&) {
    }

    try {
        if (cli_parser["version"].present()) {
            auto [major, minor, rev] = app.get()->getVersion();
            std::cout << app.get()->getName() << "\nv" << major << "." << minor << "." << rev << std::endl;
            return static_cast<int>(ExitCode::CLI_SUCCESS);
        }
    } catch (bes::cli::NoSuchArgumentException&) {
    }

    return static_cast<int>(ExitCode::SUCCESS);
}

template <class AppT>
void Kernel<AppT>::LoadContainer()
{
    std::string const key = app.get()->getKey();

    // Application key
    container.emplace<std::string>(BES_DATA_APP_KEY, key);

    // Server listen port
    unsigned listen_port = config.getOr<unsigned>(BES_SERVER_DEFAULT_PORT, "server", "listen");
    container.emplace<unsigned>(BES_DATA_LISTEN_PORT, listen_port);
}

template <class AppT>
std::string Kernel<AppT>::usage()
{
    return app.get()->usage();
}

template <class AppT>
int Kernel<AppT>::ExitRequestStatus()
{
    return exit_request.load();
}

template <class AppT>
void Kernel<AppT>::WaitForExit()
{
    std::unique_lock<std::mutex> lock(exit_cv_mutex);
    exit_cv.wait(lock);
}

template <class AppT>
template <class R>
bool Kernel<AppT>::WaitForExit(std::chrono::duration<R> const& time)
{
    std::unique_lock<std::mutex> lock(exit_cv_mutex);
    exit_cv.wait_for(lock, time);
    return exit_request.load() > 0;
}

template <class AppT>
void Kernel<AppT>::Shutdown()
{
    // Send a kill signal to our process group, this will allow a graceful shutdown like it was requested externally.
    // NB: using [std::]raise() will not reach the sigwait() handler.
    ::kill(0, SIGTERM);
}

template <class AppT>
int Kernel<AppT>::Execute(std::function<int()> const& builder)
{
    if (int r = init()) {
        return r < 0 ? 0 : r;
    }

    if (builder != nullptr) {
        if (int r = builder()) {
            return r < 0 ? 0 : r;
        }
    }

    return run();
}

template <class AppT>
int Kernel<AppT>::ErrorControlledExecution(std::function<int()> const& f)
{
    try {
        return f();

    } catch (::bes::SilentExitException& e) {
        // Managed exit, output nothing
        return e.code();

    } catch (::bes::ManagedExitException& e) {
        // Managed exit, will let the Exception control the error message entirely
        std::cerr << e.message() << std::endl;

        return e.code();

    } catch (::bes::cli::CliException& e) {
        // CLI parse error
        std::cerr << "Command-line error:\n   " << e.message() << "\n\n" << usage() << std::endl;

        return e.code();

    } catch (::bes::BesException& e) {
        // Uncaught Bes exception
        BES_LOG(FATAL) << "Fatal error: " << e.message();
        std::cerr << "Exit due to fatal error" << std::endl;
        return e.code();

    } catch (::std::exception& e) {
        // Uncaught non-Bes exception
        BES_LOG(FATAL) << "Fatal error: " << e.what();
        std::cerr << "Exit due to fatal error" << std::endl;
        return static_cast<int>(ExitCode::GEN_ERR);
    }
}

template <class AppT>
Kernel<AppT>::~Kernel()
{
    if (signal_thread.joinable()) {
        if (ExitRequestStatus() == 0) {
            // The signal listener is instructed to ignore SIGUSR1 and break out silently
            ::kill(0, SIGUSR1);
        }

        signal_thread.join();
    }
}

template <class AppT>
void Kernel<AppT>::WaitForDI()
{
    // Wait until the discovery interface is ready before starting the application's Run() sequence
    auto di = di_t::getDiscoveryInterface().get();
    unsigned wait_time = 0;
    unsigned max_wait = getConfig().template getOr<unsigned>(60, "discovery", "ready-wait-time");
    while (!di->ready()) {
        int sig = ExitRequestStatus();
        if (sig > 0) {
            throw ManagedExitException("Exit before kernel came online", signalToExitCode(sig));
        } else if (++wait_time > max_wait) {
            BES_LOG(FATAL) << "Service-discovery interface never came online";
            throw KernelException("No discovery interface");
        } else {
            if (wait_time % 10 == 0) {
                BES_LOG(NOTICE) << "Kernel waiting for discovery interface..";
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
}

}  // namespace bes::app
