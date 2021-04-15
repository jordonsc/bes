#ifndef BES_DBAL_WIDE_CASSANDRA_RESULT_H
#define BES_DBAL_WIDE_CASSANDRA_RESULT_H

#include "../exception.h"

namespace bes::dbal::wide {

template <class IteratorT, class DataT>
class Result
{
   public:
    Result() = delete;
    explicit Result(DataT&& r);

    [[nodiscard]] IteratorT begin() const;
    [[nodiscard]] IteratorT end() const;

    [[nodiscard]] size_t RowCount() const;

   private:
    DataT data;
};

template <class IteratorT, class DataT>
inline Result<IteratorT, DataT>::Result(DataT&& r) : data(std::forward<DataT>(r))
{}

template <class IteratorT, class DataT>
inline IteratorT Result<IteratorT, DataT>::begin() const
{
    return IteratorT(data);
}

template <class IteratorT, class DataT>
inline IteratorT Result<IteratorT, DataT>::end() const
{
    return IteratorT();
}

template <class IteratorT, class DataT>
inline size_t Result<IteratorT, DataT>::RowCount() const
{
    throw bes::dbal::DbalException("Unknown result counter");
}

}  // namespace bes::dbal::wide

#endif
