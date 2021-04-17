#pragma once

#include <string>

namespace bes::dbal::wide {

enum class Datatype : char
{
    Null,
    Text,
    Boolean,
    Int32,
    Int64,
    Float32,
    Float64,

    // TODO: native support for these, too:
    // Binary,  // String as a blob
    // List,    // Stored as a string in the DB
    // Map,     // Stored as a string in the DB

};

using Text = std::string;
using Boolean = bool;
using Int32 = int32_t;
using Int64 = int64_t;
using Float32 = float;
using Float64 = double;

}  // namespace bes::dbal::wide
