#pragma once

#include <string>

namespace bes::templating::data {
class Context;
}

namespace bes::templating {

class RenderingInterface
{
   public:
    virtual std::string render(std::string const& name, data::Context& context) = 0;
};

}  // namespace bes::templating
