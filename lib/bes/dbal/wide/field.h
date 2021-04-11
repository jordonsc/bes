#ifndef BES_DBAL_WIDE_FIELD_H
#define BES_DBAL_WIDE_FIELD_H

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

#endif
