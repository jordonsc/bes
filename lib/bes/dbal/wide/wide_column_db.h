#pragma once

#include "../context.h"
#include "result_future.h"
#include "schema.h"
#include "success_future.h"
#include "value.h"

using bes::dbal::Context;

namespace bes::dbal::wide {

class WideColumnDb
{
   public:
    WideColumnDb() = delete;
    explicit WideColumnDb(Context c) : context(std::move(c)) {}

    virtual SuccessFuture createTable(std::string const& table_name, Schema const& schema, bool if_not_exists)
        const = 0;
    virtual SuccessFuture dropTable(std::string const& table_name, bool if_exists) const = 0;
    virtual SuccessFuture insert(std::string const& table_name, ValueList values) const = 0;
    virtual SuccessFuture update(std::string const& table_name, Value const& key, ValueList values) const = 0;
    virtual ResultFuture retrieve(std::string const& table_name, Value const& key) const = 0;
    virtual ResultFuture retrieve(std::string const& table_name, Value const& key, FieldList fields) const = 0;
    virtual SuccessFuture remove(std::string const& table_name, Value const& key) const = 0;
    virtual SuccessFuture truncate(std::string const& table_name) const = 0;

   protected:
    [[nodiscard]] Context const& getContext() const
    {
        return context;
    }

   private:
    Context context;
};

}  // namespace bes::dbal::wide
