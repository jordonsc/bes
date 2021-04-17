#pragma once

#include "../exception.h"
#include "row.tcc"

namespace bes::dbal::wide {

template <class IteratorT, class DataT>
class Result
{
   public:
    Result() = delete;
    explicit Result(DataT&& r, std::vector<Field> fields);

    [[nodiscard]] IteratorT begin() const;
    [[nodiscard]] IteratorT end() const;

    [[nodiscard]] size_t rowCount() const;
    [[nodiscard]] size_t columnCount() const;

    [[nodiscard]] Field const& getColumn(size_t n) const;
    [[nodiscard]] std::vector<Field> const& getColumns() const;

    [[nodiscard]] auto getFirstRow() -> decltype(*begin()) const;

   private:
    DataT data;
    std::vector<Field> fields;
};

template <class IteratorT, class DataT>
inline Result<IteratorT, DataT>::Result(DataT&& r, std::vector<Field> f)
    : data(std::forward<DataT>(r)), fields(std::move(f))
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
inline size_t Result<IteratorT, DataT>::rowCount() const
{
    throw bes::dbal::DbalException("Unknown result counter");
}

template <class IteratorT, class DataT>
Field const& Result<IteratorT, DataT>::getColumn(size_t n) const
{
    return fields[n];
}

template <class IteratorT, class DataT>
std::vector<Field> const& Result<IteratorT, DataT>::getColumns() const
{
    return fields;
}

template <class IteratorT, class DataT>
size_t Result<IteratorT, DataT>::columnCount() const
{
    return fields.size();
}

template <class IteratorT, class DataT>
auto Result<IteratorT, DataT>::getFirstRow() -> decltype(*begin()) const
{
    if (rowCount() == 0) {
        throw NoRecordException("Result set contains no records");
    }

    return *begin();
}

}  // namespace bes::dbal::wide
