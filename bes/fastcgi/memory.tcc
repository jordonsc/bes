#ifndef BES_FASTCGI_MEMORY_H
#define BES_FASTCGI_MEMORY_H

#include <stdint.h>

#include "exception.h"
#include "bes/log.h"
#include "model.h"

#define bes_endian_u16(x, to_host) to_host ? be16toh(x) : htobe16(x)
#define bes_endian_32(x, to_host) swap_int32(x)
#define bes_endian_u32(x, to_host) to_host ? be32toh(x) : htobe32(x)
#define bes_endian_u64(x, to_host) to_host ? be64toh(x) : htobe64(x)

namespace bes::fastcgi {

inline int32_t swap_int32(int32_t val)
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
    val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF);
    return (val << 16) | ((val >> 16) & 0xFFFF);
#else
    return val;
#endif
}

template <class T>
inline void Endian(T& item, bool to_host);

template <>
inline void Endian(model::Header& item, bool to_host)
{
    item.content_length = bes_endian_u16(item.content_length, to_host);
    item.request_id = bes_endian_u16(item.request_id, to_host);
}

template <>
inline void Endian(model::BeginRequest& item, bool to_host)
{
    item.role = static_cast<model::Role>(bes_endian_u16(static_cast<uint16_t>(item.role), to_host));
}

template <>
inline void Endian(model::EndRequest& item, bool to_host)
{
    item.app_status = bes_endian_32(item.app_status, to_host);
}

template <class T>
inline void Endian(T& item, bool to_host)
{
    BES_LOG(CRITICAL) << "Endian shift on foreign object";
    throw FastCgiException("Unknown object to perform endian shift");
}

}  // namespace bes::fastcgi

#endif
