#pragma once

#include <memory>
#include <utility>

#include "result_interface.h"
#include "result_iterator.h"

namespace bes::dbal::wide {

/**
 * Wrapper that accepts an implemented ResultInterface object in the form of a shared pointer.
 *
 * The purpose of this class is to create a RAII approach to an abstracted object that would otherwise need to be a
 * pointer.
 */
class ResultFuture
{
   public:
    explicit ResultFuture(std::shared_ptr<bes::dbal::wide::ResultInterface> result) : result(std::move(result)) {}

    [[nodiscard]] ResultIterator begin() const
    {
        return ResultIterator(result);
    }

    [[nodiscard]] ResultIterator end() const
    {
        return ResultIterator();
    }

    [[nodiscard]] inline size_t rowCount() const
    {
        return result->rowCount();
    }

    [[nodiscard]] inline size_t columnCount() const
    {
        return result->columnCount();
    }

    [[nodiscard]] Field const& getColumn(size_t n) const
    {
        return result->getColumn(n);
    }

    [[nodiscard]] FieldList const& getColumns() const
    {
        return result->getColumns();
    }

    /**
     * Get Cell at position n from current row.
     */
    [[nodiscard]] inline Cell operator[](size_t n) const
    {
        return row()->at(n);
    }

    /**
     * Get Cell at position n from current row.
     */
    [[nodiscard]] inline Cell at(size_t n) const
    {
        return row()->at(n);
    }

    /**
     * Get Cell by name from current row.
     */
    [[nodiscard]] inline Cell at(std::string const& ns, std::string const& qualifier) const
    {
        return row()->at(ns, qualifier);
    }

    /**
     * Wait for query completion. If the query has already completed, return immediately.
     *
     * All other result functions will first call wait to ensure the result is ready.
     */
    inline void wait()
    {
        result->wait();
    }

    /**
     * Retrieve a new row from the server, invalidating previous row records.
     *
     * Returns false is no new records are available.
     */
    inline bool pop()
    {
        return result->pop();
    }

    /**
     * Returns the current row. Does not advance the internal iterator.
     *
     * Returns a nullptr if there is no row data available.
     */
    [[nodiscard]] inline Row const* row() const
    {
        return result->row();
    }

    [[nodiscard]] inline ResultInterface* operator->() const
    {
        return result.get();
    }

    [[nodiscard]] inline ResultInterface* get() const
    {
        return result.get();
    }

   protected:
    std::shared_ptr<bes::dbal::wide::ResultInterface> result;
};

}  // namespace bes::dbal::wide