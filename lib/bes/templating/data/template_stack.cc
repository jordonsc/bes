#include "template_stack.h"

using namespace bes::templating::data;

TemplateStack::TemplateStack(bes::templating::RenderingInterface* eng) : rendering_engine(eng)
{
    // The top-most template MUST always be a nullptr, indicating there are no more children
    PrependTemplate(nullptr);
}

void TemplateStack::PrependTemplate(bes::templating::node::RootNode const* tmp)
{
    template_stack.push_front(tmp);
    template_iterator = template_stack.begin();
}

bes::templating::node::RootNode const* TemplateStack::GetChildTemplate() const
{
    return *template_iterator;
}

bes::templating::node::RootNode const* TemplateStack::GetNextChildTemplate()
{
    ++template_iterator;
    return *(template_iterator--);
}

void TemplateStack::NextChild()
{
    ++template_iterator;
}

void TemplateStack::PrevChild()
{
    --template_iterator;
}

void TemplateStack::Engine(bes::templating::RenderingInterface* e)
{
    rendering_engine = e;
}

bes::templating::RenderingInterface* TemplateStack::Engine() const
{
    return rendering_engine;
}
