#ifndef BES_DBAL_WIDE_CELL_H
#define BES_DBAL_WIDE_CELL_H

#include <any>
#include <string>

#include "field.h"

namespace bes::dbal::wide {

class Cell
{
   public:
    Cell(Field&& field, std::any&& data);

    [[nodiscard]] Field const& GetField();

    template <class T>
    [[nodiscard]] T const& Get() const&;

    template <class T>
    [[nodiscard]] T&& Get() &&;

   protected:
    Field field;
    std::any data;
};

Cell::Cell(Field&& field, std::any&& data) : field(std::forward<Field>(field)), data(std::forward<std::any>(data)) {}

Field const& Cell::GetField()
{
    return field;
}

template <class T>
T const& Cell::Get() const&
{
    return std::any_cast<data>;
}

template <class T>
T&& Cell::Get() &&
{
    return std::any_cast<T>(data);
}

}  // namespace bes::dbal::wide

#endif
