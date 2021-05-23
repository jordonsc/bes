#pragma once

#include <utility>

#include "row.h"

namespace bes::dbal::wide {

/**
 * Abstract result object (should be a future) that should be extended by the DBAL implementation.
 */
class ResultInterface
{
   public:
    [[nodiscard]] virtual size_t rowCount() = 0;
    [[nodiscard]] virtual size_t columnCount() = 0;

    [[nodiscard]] virtual Field const& getColumn(size_t n) = 0;
    [[nodiscard]] virtual FieldList const& getColumns() = 0;

    /**
     * Wait for query completion. If the query has already completed, return immediately.
     *
     * All other result functions will first call wait to ensure the result is ready.
     */
    virtual void wait() = 0;

    /**
     * Retrieves a new row record, invalidating the current row.
     *
     * Returns true if a row is found, false if there are no further records. Get row data from row() after calling
     * pop().
     */
    virtual bool pop() = 0;

    /**
     * Returns the current row.
     *
     * Does not advance the internal iterator. Returns a nullptr if there is no row data available. A call to pop()
     * must precede this call.
     */
    [[nodiscard]] virtual Row const* row() = 0;
};

}  // namespace bes::dbal::wide
