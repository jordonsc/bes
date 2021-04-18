#pragma once

#include <string>

#include "cell.tcc"

namespace bes::dbal::wide {

class Row
{
   public:
    [[nodiscard]] virtual Cell at(size_t) const = 0;
    [[nodiscard]] virtual Cell at(std::string const& ns, std::string const& qualifier) const = 0;
};

}  // namespace bes::dbal::wide
