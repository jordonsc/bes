#pragma once

#include "../context_db.h"
#include "result_future.h"
#include "schema.h"
#include "success_future.h"
#include "value.h"

using bes::dbal::Context;

namespace bes::dbal::wide {

class WideColumnDb : public ContextualDatabase
{
   public:
    WideColumnDb() = delete;
    explicit WideColumnDb(Context c) : ContextualDatabase(std::move(c)) {}

    /**
     * Create a table.
     *
     * @throws AlreadyExistsException if the table exists.
     */
    virtual SuccessFuture createTable(std::string const& table_name, Schema const& schema) = 0;

    /**
     * Drop a table.
     *
     * @throws DoesNotExistException if the table does not exist.
     */
    virtual SuccessFuture dropTable(std::string const& table_name) = 0;

    /**
     * Write an object to the table. Will overwrite any existing record.
     */
    virtual SuccessFuture apply(std::string const& table_name, Value const& key, ValueList values) = 0;

    /**
     * Retrieve an object, returning all of its columns.
     */
    virtual ResultFuture retrieve(std::string const& table_name, Value const& key) = 0;

    /**
     * Retrieve an object, returning a selection of its columns.
     */
    virtual ResultFuture retrieve(std::string const& table_name, Value const& key, FieldList fields) = 0;

    /**
     * Delete an object, if it does not exist, nothing will happen.
     */
    virtual SuccessFuture remove(std::string const& table_name, Value const& key) = 0;

    /**
     * Truncate the entire table.
     *
     * @throws DoesNotExistException if the table does not exist.
     */
    virtual SuccessFuture truncate(std::string const& table_name) = 0;
};

}  // namespace bes::dbal::wide
