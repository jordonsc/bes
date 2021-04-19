#pragma once

#include <memory>
#include <utility>

#include "result.h"
#include "result_iterator.h"

namespace bes::dbal::wide {

/**
 * Wrapper that accepts an abstracted Result object as a pointer and takes it under memory management.
 */
class ResultFuture
{
   public:
    explicit ResultFuture(std::shared_ptr<bes::dbal::wide::Result> result) : result(std::move(result)) {}

    ResultIterator begin() const
    {
        return ResultIterator(result);
    }

    ResultIterator end() const
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
     * Retrieves a row and returns it, invalidating the previous row.
     *
     * Will return a nullptr if no further rows exist.
     */
    inline Row const* pop()
    {
        return result->pop();
    }

    /**
     * Returns the current row. Does not advance the internal iterator.
     *
     * Returns a nullptr is pop() has never been called or if there is no more row data available.
     */
    [[nodiscard]] inline Row const* row() const
    {
        return result->row();
    }

    [[nodiscard]] inline Result* operator->() const
    {
        return result.get();
    }

    [[nodiscard]] inline Result* get() const
    {
        return result.get();
    }

   protected:
    std::shared_ptr<bes::dbal::wide::Result> result;
};

}  // namespace bes::dbal::wide