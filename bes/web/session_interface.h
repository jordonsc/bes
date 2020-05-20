#ifndef BES_WEB_SESSION_INTERFACE_H
#define BES_WEB_SESSION_INTERFACE_H

#include "session.h"

namespace bes::web {

class SessionInterface
{
   public:
    /**
     * Generate a unique session ID and create a blank session.
     */
    virtual Session CreateSession() = 0;

    /**
     * Get a session.
     *
     * If the session does not exist, throw a SessionNotExistsException.
     */
    virtual Session GetSession(std::string const& id) = 0;

    /**
     * Persist the session, resetting its TTL.
     *
     * If the session does not contain a session ID, create a new session and persist it with this session data.
     */
    virtual void PersistSession(Session const& session) = 0;

    /**
     * Define the TTL used when persisting sessions.
     */
    virtual void SetSessionTtl(uint64_t ttl) = 0;
};

}  // namespace bes::web

#endif
