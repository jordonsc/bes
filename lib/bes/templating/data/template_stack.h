#pragma once

#include <list>

#include "../exception.h"
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
    void prependTemplate(bes::templating::node::RootNode const* tmp);
    [[nodiscard]] bes::templating::node::RootNode const* getChildTemplate() const;
    [[nodiscard]] bes::templating::node::RootNode const* getNextChildTemplate();
    void nextChild();
    void prevChild();

    /// Rendering engine for includes
    void engine(bes::templating::RenderingInterface* e);
    [[nodiscard]] bes::templating::RenderingInterface* engine() const;

   protected:
    bes::templating::RenderingInterface* rendering_engine;
    std::list<node::RootNode const*> template_stack;
    std::list<node::RootNode const*>::iterator template_iterator;
};

}  // namespace bes::templating::data

