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
};

using Text = std::string;
using Boolean = bool;
using Int32 = int32_t;
using Int64 = int64_t;
using Float32 = float;
using Float64 = double;

using TextList = std::vector<Text>;
using Int32List = std::vector<Int32>;
using Int64List = std::vector<Int64>;
using Float32List = std::vector<Float32>;
using Float64List = std::vector<Float64>;

}  // namespace bes::dbal::wide
