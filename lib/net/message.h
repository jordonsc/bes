#ifndef BES_NET_SOCKET_MESSAGE_H
#define BES_NET_SOCKET_MESSAGE_H

#include <string>

#include "address.h"

namespace bes::net {

struct Message
{
    inline Message(std::string payload, bes::net::Address address) : payload(payload), address(address) {}

    std::string payload;
    bes::net::Address address;
};

}  // namespace bes::net

#endif
