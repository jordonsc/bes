#ifndef BES_NET_ADDRESS_H
#define BES_NET_ADDRESS_H

#include <string>
#include <utility>

namespace bes::net {

/**
 * Represents an address and port by which a service can be accessed on.
 *
 * Optionally, this structure can hold an IPv4 and IPv6 address, however using the IPv4 address as a hostname or address
 * is typical usage.
 */
class Address
{
   public:
    Address(std::string ip4Addr, uint16_t addrPort)
        : ip4_addr(std::move(ip4Addr)),
          addr_port(addrPort),
          full_addr_ip4(std::string(ip4_addr + ":" + std::to_string(addr_port)))
    {}

    Address(std::string ip4Addr, std::string ip6Addr, uint16_t addrPort)
        : ip4_addr(std::move(ip4Addr)),
          ip6_addr(std::move(ip6Addr)),
          addr_port(addrPort),
          full_addr_ip4(std::string(ip4_addr + ":" + std::to_string(addr_port))),
          full_addr_ip6(std::string(ip6_addr + ":" + std::to_string(addr_port)))
    {}

    Address(Address const&) = default;
    Address(Address&&) = default;
    Address& operator=(Address const&) = default;
    Address& operator=(Address&&) = default;

    bool operator==(const Address& rhs) const
    {
        return full_addr_ip4 == rhs.full_addr_ip4 && full_addr_ip6 == rhs.full_addr_ip6;
    }

    bool operator!=(const Address& rhs) const
    {
        return !(rhs == *this);
    }

    [[nodiscard]] inline bool HasIp4Addr() const
    {
        return !ip4_addr.empty();
    }

    [[nodiscard]] inline bool HasIp6Addr() const
    {
        return !ip6_addr.empty();
    }

    [[nodiscard]] inline std::string const& Ip4Addr() const
    {
        return ip4_addr;
    }

    [[nodiscard]] inline std::string const& Ip6Addr() const
    {
        return ip6_addr;
    }

    [[nodiscard]] inline uint16_t const& Port() const
    {
        return addr_port;
    }

    [[nodiscard]] inline std::string const& Ip4AddrFull() const
    {
        return full_addr_ip4;
    }

    [[nodiscard]] inline std::string const& Ip6AddrFull() const
    {
        return full_addr_ip6;
    }

    [[nodiscard]] inline std::string const& Addr() const
    {
        return ip6_addr.empty() ? ip4_addr : ip6_addr;
    }

    [[nodiscard]] inline std::string const& AddrFull() const
    {
        return ip6_addr.empty() ? full_addr_ip4 : full_addr_ip6;
    }

   protected:
    std::string ip4_addr;
    std::string ip6_addr;
    uint16_t addr_port;
    std::string full_addr_ip4;
    std::string full_addr_ip6;
};

}  // namespace bes::net

#endif
