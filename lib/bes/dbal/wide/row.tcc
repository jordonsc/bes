#ifndef BES_DBAL_WIDE_CASSANDRA_ROW_H
#define BES_DBAL_WIDE_CASSANDRA_ROW_H

#include <utility>

namespace bes::dbal::wide {

template <class IteratorT, class DataT>
class Row
{
   public:
    Row() = delete;
    explicit Row(DataT&& row);

    [[nodiscard]] IteratorT begin() const;
    [[nodiscard]] IteratorT end() const;

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

}  // namespace bes::dbal::wide

#endif