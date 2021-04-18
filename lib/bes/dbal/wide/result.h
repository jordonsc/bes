#pragma once

#include <utility>

#include "row.h"

namespace bes::dbal::wide {

/**
 * Abstract result object (should be a future) that should be extended by the DBAL implementation.
 */
class Result
{
   public:
    [[nodiscard]] virtual size_t rowCount() const = 0;
    [[nodiscard]] virtual size_t columnCount() const = 0;

    [[nodiscard]] virtual Field const& getColumn(size_t n) const = 0;
    [[nodiscard]] virtual FieldList const& getColumns() const = 0;

    /**
     * Wait for query completion. If the query has already completed, return immediately.
     *
     * All other result functions will first call wait to ensure the result is ready.
     */
    virtual void wait() const = 0;

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
    [[nodiscard]] virtual Row const* row() const = 0;
};

}  // namespace bes::dbal::wide
