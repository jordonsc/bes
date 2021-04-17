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

    [[nodiscard]] Cell operator[](size_t) const;

    template <class ReturnType>
    [[nodiscard]] ReturnType value(std::string const& ns, std::string const& qualifier) const;

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
Cell Row<IteratorT, DataT>::operator[](size_t) const
{
    throw bes::dbal::DbalException("Cannot index row on unknown template type");
}

template <class IteratorT, class DataT>
template <class ReturnType>
ReturnType Row<IteratorT, DataT>::value(std::string const& ns, std::string const& qualifier) const
{
    throw bes::dbal::DbalException("Cannot discern value of unknown row template");
}

}  // namespace bes::dbal::wide
