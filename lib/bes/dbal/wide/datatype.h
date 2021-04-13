#ifndef BES_DBAL_WIDE_DATATYPE_H
#define BES_DBAL_WIDE_DATATYPE_H

namespace bes::dbal::wide {

enum class Datatype : char
{
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

}

#endif