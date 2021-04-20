#pragma once

#include <byteswap.h>

#include <cstdint>

namespace bes::fastcgi::model {

/// FastCGI version
static constexpr int fcgi_version = 1;

/// All FastCGI records are a multiple of the chunk_size
static constexpr int chunk_size = 8;

struct Request
{
    uint16_t request_id;
    int socket;
};

/**
 * FastCGI record types, these are the types of messages that can be sent with each record of the FCGI request.
 */
enum class RecordType : uint8_t
{
    BEGIN_REQUEST = 1,      // In
    ABORT_REQUEST = 2,      // In
    END_REQUEST = 3,        // Out
    PARAMS = 4,             // In: env variables
    IN = 5,                 // In: post data
    OUT = 6,                // Out: response
    ERR = 7,                // Out: errors
    DATA = 8,               // In: filter data
    GET_VALUES = 9,         // In
    GET_VALUES_RESULT = 10  // Out
};

/**
 * FastCGI roles.
 *
 * Each request will specify one of these roles. Each should respond differently, however the only role of real interest
 * is the Responder role, which behaves in the way you'd expect: _here is a request, give me a response_.
 *
 * See: http://www.mit.edu/~yandros/doc/specs/fcgi-spec.html#S6.1
 */
enum class Role : uint16_t
{
    /// Give me a response for the given request
    RESPONDER = 1,

    /// Given the supplied parameters, tell me if this request is authorised or not
    AUTHORIZER = 2,

    /// Generate a filtered response similar to a Responder
    FILTER = 3
};

/**
 * A status code we return to the FCGI server upon completion.
 *
 * This is different to the HTTP status code, which should be sent in the form of a `Status` header, instead.
 */
enum class ProtoStatus : uint8_t
{
    REQUEST_COMPLETE = 0,
    CANT_MPX_CONN = 1,
    OVERLOADED = 2,
    UNKNOWN_ROLE = 3
};

/**
 * A record header.
 *
 * Each record the server sends will have this data-structure proceeding it, it allows us to prepare for the proceeding
 * record.
 */
struct Header
{
    uint8_t version;
    RecordType type;
    uint16_t request_id;
    uint16_t content_length;
    uint8_t padding_length;
    uint8_t reserved;
};

/**
 * Record: BeginRequest
 *
 * This should be the first record the server sends, it defines which Role the request is expecting and sets some flags.
 */
struct BeginRequest
{
    Role role;
    uint8_t flags;
    uint8_t reserved[5];
};

/**
 * Record: EndRequest
 */
struct EndRequest
{
    int32_t app_status;
    ProtoStatus protocol_status;
    uint8_t reserved[3];
};

}  // namespace bes::fastcgi::model

