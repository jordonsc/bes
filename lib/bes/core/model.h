#ifndef BES_BES_MODEL_H
#define BES_BES_MODEL_H

#include <csignal>
#include <memory>
#include <stdexcept>
#include <tuple>

/**
 * Set of keys used in the kernel's container
 */
#define BES_DATA_LISTEN_PORT "data.listen-port"
#define BES_DATA_APP_KEY "data.app-key"

/**
 * Default listen port for a server (RPC or otherwise)
 *
 * Config override: server.listen
 */
#ifndef BES_SERVER_DEFAULT_PORT
#define BES_SERVER_DEFAULT_PORT 60001
#endif

namespace bes {

using version_t = std::tuple<std::uint16_t, std::uint16_t, std::uint16_t>;

/**
 * Generic application exit codes.
 *
 * All application-specific codes should be kept >= 100.
 */
enum class ExitCode : int
{
    CLI_SUCCESS = -1,        // OK state for CLI intercept (--help, --version), will be converted to ExitCode::SUCCESS
    SUCCESS = EXIT_SUCCESS,  // 0
    GEN_ERR = EXIT_FAILURE,  // 1
    CLI_PARSE_ERR = 2,       // App failed to parse CLI
    CONFIG_ERR = 3,          // Something went wrong loading config
    USER_EXIT = 10,          // SIGINT
    TERMINATED = 11,         // SIGTERM
};

inline int SignalToExitCode(int signal)
{
    switch (signal) {
        case 0:
            return static_cast<int>(ExitCode::SUCCESS);
        case SIGINT:
            return static_cast<int>(ExitCode::USER_EXIT);
        case SIGTERM:
            return static_cast<int>(ExitCode::TERMINATED);
        default:
            return static_cast<int>(ExitCode::GEN_ERR);
    }
}

}  // namespace bes

#endif  // BES_MODEL_H
