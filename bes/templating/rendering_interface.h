#ifndef BES_TEMPLATING_RENDERING_INTERFACE_H
#define BES_TEMPLATING_RENDERING_INTERFACE_H

#include <string>

namespace bes::templating::data {
class Context;
}

namespace bes::templating {

class RenderingInterface
{
   public:
    virtual std::string Render(std::string const& name, data::Context& context) = 0;
};

}  // namespace bes::templating

#endif
