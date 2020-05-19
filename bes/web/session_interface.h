#ifndef BES_WEB_SESSION_INTERFACE_H
#define BES_WEB_SESSION_INTERFACE_H

#include "session.h"

namespace bes::web {

class SessionInterface {
   public:
    virtual Session CreateSession() = 0;
    virtual Session GetSession(std::string const& id) = 0;
    virtual void PersistSession(Session const&) = 0;
};

}


#endif