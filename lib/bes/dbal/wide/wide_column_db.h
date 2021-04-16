#pragma once

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

    virtual void createTable(std::string const& table_name, Schema const& schema, bool if_not_exists) const = 0;
    virtual void dropTable(std::string const& table_name, bool if_exists) const = 0;

    [[nodiscard]] Context const& getContext() const
    {
        return context;
    }

   protected:
    Context context;
};

}  // namespace bes::dbal::wide
