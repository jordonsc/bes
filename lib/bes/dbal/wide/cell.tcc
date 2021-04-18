#pragma once

#include <any>
#include <string>

#include "../exception.h"
#include "field.h"

namespace bes::dbal::wide {

class Cell
{
   public:
    inline Cell(Field field, std::any data);

    [[nodiscard]] inline Field const& getField() const;

    template <class T>
    [[nodiscard]] T as() const&;

    template <class T>
    [[nodiscard]] T&& as() &&;

   protected:
    Field field;
    std::any data;
};

Cell::Cell(Field field, std::any data) : field(std::move(field)), data(std::move(data)) {}

Field const& Cell::getField() const
{
    return field;
}

template <class T>
T Cell::as() const&
{
    if (field.datatype == Datatype::Null) {
        throw bes::dbal::NullValueException("Field " + field.ns + ":" + field.qualifier + " is null");
    }

    return std::any_cast<T>(data);
}

template <class T>
T&& Cell::as() &&
{
    if (field.datatype == Datatype::Null) {
        throw bes::dbal::NullValueException("Field " + field.ns + ":" + field.qualifier + " is null");
    }
    return std::any_cast<T&&>(std::move(data));
}

}  // namespace bes::dbal::wide
