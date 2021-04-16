#pragma once

#include <any>
#include <string>

#include "field.h"

namespace bes::dbal::wide {

class Cell
{
   public:
    inline Cell(Field&& field, std::any&& data);

    [[nodiscard]] inline Field const& getField() const;

    template <class T>
    [[nodiscard]] T as() const&;

    template <class T>
    [[nodiscard]] T&& as() &&;

   protected:
    Field field;
    std::any data;
};

Cell::Cell(Field&& field, std::any&& data) : field(std::forward<Field>(field)), data(std::forward<std::any>(data)) {}

Field const& Cell::getField() const
{
    return field;
}

template <class T>
T Cell::as() const&
{
    return std::any_cast<T>(data);
}

template <class T>
T&& Cell::as() &&
{
    return std::any_cast<T>(data);
}

}  // namespace bes::dbal::wide

