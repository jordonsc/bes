#include "template_stack.h"

using namespace bes::templating::data;

TemplateStack::TemplateStack(bes::templating::RenderingInterface* eng) : rendering_engine(eng)
{
    // The top-most template MUST always be a nullptr, indicating there are no more children
    prependTemplate(nullptr);
}

void TemplateStack::prependTemplate(bes::templating::node::RootNode const* tmp)
{
    template_stack.push_front(tmp);
    template_iterator = template_stack.begin();
}

bes::templating::node::RootNode const* TemplateStack::getChildTemplate() const
{
    return *template_iterator;
}

bes::templating::node::RootNode const* TemplateStack::getNextChildTemplate()
{
    ++template_iterator;
    return *(template_iterator--);
}

void TemplateStack::nextChild()
{
    ++template_iterator;
}

void TemplateStack::prevChild()
{
    --template_iterator;
}

void TemplateStack::engine(bes::templating::RenderingInterface* e)
{
    rendering_engine = e;
}

bes::templating::RenderingInterface* TemplateStack::engine() const
{
    return rendering_engine;
}
