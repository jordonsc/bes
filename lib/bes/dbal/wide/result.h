#pragma once

#include <utility>

#include "row.h"

namespace bes::dbal::wide {

class Result
{
   public:
    [[nodiscard]] virtual size_t rowCount() const = 0;
    [[nodiscard]] virtual size_t columnCount() const;

    [[nodiscard]] virtual Field const& getColumn(size_t n) const;
    [[nodiscard]] virtual FieldList const& getColumns() const;

    /**
     * Retrieves a row and returns it, invalidating the previous row.
     *
     * Will return a nullptr if no further rows exist.
     */
    virtual Row const* pop() = 0;

    /**
     * Returns the current row. Does not advance the internal iterator.
     *
     * Returns a nullptr is pop() has never been called or if there is no more row data available.
     */
    [[nodiscard]] virtual Row const* row() = 0;

   protected:
    explicit Result(FieldList fields);
    FieldList fields;
};

}  // namespace bes::dbal::wide
