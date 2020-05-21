#ifndef BES_TEMPLATING_ENGINE_H
#define BES_TEMPLATING_ENGINE_H

#include <bes/core.h>

#include <fstream>
#include <memory>
#include <shared_mutex>
#include <sstream>
#include <streambuf>
#include <unordered_map>

#include "data/context.h"
#include "data/template_stack.h"
#include "node/root_node.h"
#include "parser.h"
#include "rendering_interface.h"
#include "text.h"

namespace bes::templating {

class Engine : public RenderingInterface
{
   public:
    Engine();

    void LoadFile(std::string const& name, std::string const& filename);
    void LoadString(std::string const& name, std::string const& filename);

    /**
     * Add a function matching syntax of Filter, as a filter.
     */
    void AddFilter(std::string const& name, Filter filter);

    /**
     * Bind an object member function as a filter.
     *
     * Usage:
     *  AddFilter("myfilter", &MyClass::FilterFn, &obj_of_myclass);
     */
    template <class MethodT, class ObjT>
    void AddFilter(std::string const& name, MethodT&& method, ObjT&& object);

    /**
     * Render a template and return it as a string.
     *
     * If throw_on_error is false, rendering exceptions will be suppressed and sent to the log sink instead. If a
     * template does not exist, a MissingTemplateException will be raised regardless of throw_on_error.
     */
    std::string Render(std::string const& name, data::Context& context) override;

    bes::FileFinder search_path;

   protected:
    std::unordered_map<std::string, std::shared_ptr<node::RootNode>> templates;
    std::unordered_map<std::string, Filter> filters;
    Parser parser;
    std::shared_mutex mutex;
};

template <class MethodT, class ObjT>
inline void Engine::AddFilter(std::string const& name, MethodT&& method, ObjT&& object)
{
    filters[name] = std::bind(std::forward<MethodT>(method), std::forward<ObjT>(object), std::placeholders::_1);
}

}  // namespace bes::templating

#endif
