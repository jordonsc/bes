#pragma once

#include <string>

#include "datatype.h"

namespace bes::dbal::wide {

struct Field
{
    Datatype datatype;
    std::string ns;
    std::string qualifier;
};

}  // namespace bes::dbal::wide
