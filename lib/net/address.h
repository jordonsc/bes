#ifndef BES_NET_ADDRESS_H
#define BES_NET_ADDRESS_H

#include <string>

namespace bes::net {

/**
 * Represents an address and port by which a service can be accessed on.
 *
 * The address might be a hostname or an IP address. Note hat while both IPv4 and IPv6 addresses are in this structure,
 * they both be identical hostnames.
 *
 * If the presence of an IPv6 address is present but it is a hostname, you should assume that the hostname may resolve
 * IPv6 addresses. If it is blank, assume that the network only resolves IPv4 addresses or is unknown.
 */
class Address
{
   public:
    Address(const std::string& ip4Addr, const uint16_t addrPort)
        : ip4_addr(ip4Addr), addr_port(addrPort), full_addr_ip4(std::string(ip4_addr + ":" + std::to_string(addr_port)))
    {}
    Address(const std::string& ip4Addr, const std::string& ip6Addr, const uint16_t addrPort)
        : ip4_addr(ip4Addr),
          ip6_addr(ip6Addr),
          addr_port(addrPort),
          full_addr_ip4(std::string(ip4_addr + ":" + std::to_string(addr_port))),
          full_addr_ip6(std::string(ip6_addr + ":" + std::to_string(addr_port)))
    {}

    bool operator==(const Address& rhs) const
    {
        return full_addr_ip4 == rhs.full_addr_ip4 && full_addr_ip6 == rhs.full_addr_ip6;
    }

    bool operator!=(const Address& rhs) const
    {
        return !(rhs == *this);
    }

    bool operator<(const Address& rhs) const
    {
        if (full_addr_ip4 < rhs.full_addr_ip4)
            return true;
        if (rhs.full_addr_ip4 < full_addr_ip4)
            return false;
        return full_addr_ip6 < rhs.full_addr_ip6;
    }

    bool operator>(const Address& rhs) const
    {
        return rhs < *this;
    }

    bool operator<=(const Address& rhs) const
    {
        return !(rhs < *this);
    }

    bool operator>=(const Address& rhs) const
    {
        return !(*this < rhs);
    }

    friend void swap(Address& first, Address& second) noexcept
    {
        using std::swap;

        swap(first.ip4_addr, second.ip4_addr);
        swap(first.ip6_addr, second.ip6_addr);
        swap(first.addr_port, second.addr_port);
        swap(first.full_addr_ip4, second.full_addr_ip4);
        swap(first.full_addr_ip6, second.full_addr_ip6);
    }

    inline bool HasIp4Addr() const
    {
        return ip4_addr.length();
    }

    inline bool HasIp6Addr() const
    {
        return ip6_addr.length();
    }

    inline std::string const& Ip4Addr() const
    {
        return ip4_addr;
    }

    inline std::string const& Ip6Addr() const
    {
        return ip6_addr;
    }

    inline uint16_t const& Port() const
    {
        return addr_port;
    }

    inline std::string const& Ip4AddrFull() const
    {
        return full_addr_ip4;
    }

    inline std::string const& Ip6AddrFull() const
    {
        return full_addr_ip6;
    }

    inline std::string const& AddrFull() const
    {
        return ip6_addr.length() ? full_addr_ip6 : full_addr_ip4;
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
