#ifndef BES_DBAL_WIDE_WIDEDB_H
#define BES_DBAL_WIDE_WIDEDB_H

#include "schema.h"

namespace bes::dbal::wide {

class WideColumnDb
{
   public:
    virtual void CreateTable(std::string table_name, Schema schema, bool if_not_exists = false) = 0;
    virtual void DropTable(std::string table_name, bool if_exists = false) = 0;
};

}  // namespace bes::dbal::wide

#endif