#pragma once

namespace bes::dbal::kv::redis {

static char const* const CFG_SENTINEL_SVC = "sentinel_service";  // Sentinel service name
static char const* const CFG_HOSTNAME = "hostname";              // Direct-connect hostname
static char const* const CFG_PORT = "port";                      // Direct-connect port

static char const* const CFG_CON_TIMEOUT = "connection_timeout";   // Connection timeout in milliseconds
static char const* const CFG_MAX_RECONNECTS = "max_reconnects";    // Maximum attempts of reconnects if dropped
static char const* const CFG_RECON_BACKOFF = "reconnect_backoff";  // Reconnect backoff in milliseconds

static char const* const DEFAULT_CON_TIMEOUT = "250";
static char const* const DEFAULT_MAX_RECONNECTS = "3";
static char const* const DEFAULT_RECON_BACKOFF = "250";

static char const* const DEFAULT_PORT = "6379";

}  // namespace bes::dbal::kv::redis
