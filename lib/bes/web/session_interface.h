#pragma once

#include <random>

#include "session.h"

namespace bes::web {

class SessionInterface
{
   public:
    /**
     * Generate a unique session ID and create a blank session.
     */
    virtual Session createSession(std::string const& ns) = 0;

    /**
     * Get a session.
     *
     * If the session does not exist, throw a SessionNotExistsException.
     */
    virtual Session getSession(std::string const& id) = 0;

    /**
     * Persist the session, resetting its TTL.
     *
     * If the session does not contain a session ID, it is considered invalid and an exception raised.
     */
    virtual void persistSession(Session const& session) = 0;

    /**
     * Define the TTL used when persisting sessions.
     */
    virtual void setSessionTtl(uint64_t ttl) = 0;

    /**
     * Generate a session key.
     *
     * This should result in a big, random value that has a low chance of collisions, but you should always check for a
     * collision and re-run if you get a conflict.
     *
     * `ns` is a prefix to be used as a namespace, or cool looks - your choice.
     */
    inline static std::string generateSessionKey(std::string const& ns = "S")
    {
        std::stringstream out;
        std::mt19937 rng(std::random_device{}());
        std::uniform_int_distribution<std::mt19937::result_type> dist64(1e17, 1e18);

        // Namespace as a prefix
        out << ns;

        // We'll create a giant, random number - this should reduce collisions to a bare minimum but more importantly,
        // make this all but impossible to guess by brute-force
        out << std::hex << dist64(rng) << dist64(rng);

        return out.str();
    }
};

}  // namespace bes::web
