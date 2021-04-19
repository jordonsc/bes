#pragma once

#include <memory>

#include "row.h"

namespace bes::dbal::wide {

class RowContainer
{
   public:
    RowContainer() = delete;
    explicit RowContainer(Row const* row) : row(row) {}

    [[nodiscard]] inline Cell at(size_t n) const
    {
        return row->at(n);
    }

    [[nodiscard]] inline Cell at(std::string const& ns, std::string const& qualifier) const
    {
        return row->at(ns, qualifier);
    }

    [[nodiscard]] Row const* operator->() const
    {
        return row;
    };

    [[nodiscard]] Row const& operator*() const
    {
        return *row;
    };

    [[nodiscard]] Row const* get() const
    {
        return row;
    }

   private:
    Row const* row;
};

}  // namespace bes::dbal::wide