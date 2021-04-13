#ifndef BES_DBAL_WIDE_WIDEDB_H
#define BES_DBAL_WIDE_WIDEDB_H

#include "../context.h"
#include "schema.h"

using bes::dbal::Context;

namespace bes::dbal::wide {

class WideColumnDb
{
   public:
    explicit WideColumnDb(Context&& c) : context(std::move(c)) {}
    explicit WideColumnDb(Context const& c) : context(c) {}
    WideColumnDb() {}

    virtual void CreateTable(std::string const& table_name, Schema const& schema, bool if_not_exists) const = 0;
    virtual void DropTable(std::string const& table_name, bool if_exists) const = 0;

    [[nodiscard]] Context const& GetContext() const
    {
        return context;
    }

   protected:
    Context context;
};

}  // namespace bes::dbal::wide

#endif