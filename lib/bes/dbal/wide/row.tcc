#pragma once

#include <string>
#include <utility>

#include "../exception.h"
#include "cell.tcc"

namespace bes::dbal::wide {

template <class IteratorT, class DataT>
class Row
{
   public:
    Row() = delete;
    explicit Row(DataT&& row);

    [[nodiscard]] IteratorT begin() const;
    [[nodiscard]] IteratorT end() const;

    Cell operator[](size_t);
    Cell operator[](std::string const&);

   private:
    DataT data;
};

template <class IteratorT, class DataT>
inline Row<IteratorT, DataT>::Row(DataT&& d) : data(std::forward<DataT>(d))
{}

template <class IteratorT, class DataT>
inline IteratorT Row<IteratorT, DataT>::begin() const
{
    return IteratorT(data);
}

template <class IteratorT, class DataT>
inline IteratorT Row<IteratorT, DataT>::end() const
{
    return IteratorT();
}

template <class IteratorT, class DataT>
Cell Row<IteratorT, DataT>::operator[](size_t)
{
    throw bes::dbal::DbalException("Cannot index row on unknown template type");
}

template <class IteratorT, class DataT>
Cell Row<IteratorT, DataT>::operator[](const std::string&)
{
    throw bes::dbal::DbalException("Cannot index row on unknown template type");
}

}  // namespace bes::dbal::wide
