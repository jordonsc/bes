#ifndef BES_TEMPLATING_DATA_TEMPLATE_STACK_H
#define BES_TEMPLATING_DATA_TEMPLATE_STACK_H

#include <list>

#include "../rendering_interface.h"

namespace bes::templating::node {
class RootNode;
}  // namespace bes::templating::node

namespace bes::templating::data {

class TemplateStack
{
   public:
    explicit TemplateStack(RenderingInterface*);

   public:
    /// Stack of templates used when extending templates
    void PrependTemplate(bes::templating::node::RootNode const* tmp);
    [[nodiscard]] bes::templating::node::RootNode const* GetChildTemplate() const;
    void NextChild();
    void PrevChild();

    /// Rendering engine for includes
    void Engine(bes::templating::RenderingInterface* engine);
    [[nodiscard]] bes::templating::RenderingInterface* Engine() const;

   protected:
    bes::templating::RenderingInterface* rendering_engine;
    std::list<node::RootNode const*> template_stack;
    std::list<node::RootNode const*>::iterator template_iterator;
};

}  // namespace bes::templating::data

#endif
