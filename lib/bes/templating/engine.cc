#include "engine.h"

#include <utility>

using namespace bes::templating;

Engine::Engine()
{
    addFilter("trim", [](std::string& txt) -> void {
        Text::trim(txt);
    });

    addFilter("ltrim", [](std::string& txt) -> void {
        Text::trimFront(txt);
    });

    addFilter("rtrim", [](std::string& txt) -> void {
        Text::trimBack(txt);
    });

    addFilter("nl2br", [](std::string& txt) -> void {
        Text::nl2br(txt);
    });

    addFilter("nl2p", [](std::string& txt) -> void {
        Text::nl2p(txt);
    });
}

void Engine::loadFile(std::string const& name, std::string const& filename)
{
    std::unique_lock<std::shared_mutex> lock;

    if (search_path.empty()) {
        search_path.AppendSearchPath(".");
    }

    std::string file_path = search_path.findInPath(filename);

    std::ifstream str(file_path);
    std::stringstream buffer;
    buffer << str.rdbuf();

    auto ptr = std::make_shared<node::RootNode>(name);
    parser.parse(*ptr, buffer.str());

    ptr->filters = &filters;
    templates.insert_or_assign(name, ptr);

    BES_LOG(DEBUG) << "Loaded template '" << name << "' from file '" << file_path << "'";
}

void Engine::loadString(std::string const& name, std::string const& data)
{
    std::unique_lock<std::shared_mutex> lock;

    auto ptr = std::make_shared<node::RootNode>(name);
    parser.parse(*ptr, data);

    ptr->filters = &filters;
    templates.insert_or_assign(name, ptr);

    BES_LOG(DEBUG) << "Loaded template '" << name << "' from memory";
}

std::string Engine::render(std::string const& name, data::Context& context)
{
    std::shared_lock<std::shared_mutex> lock;

    std::ostringstream str;
    std::shared_ptr<node::RootNode const> root;
    std::unordered_map<std::string, bool> recursion_check;
    recursion_check[name] = true;

    // Build a template stack, used during node rendering when encountering child templates or includes
    data::TemplateStack ts(this);
    ts.prependTemplate(nullptr);

    do {
        try {
            if (root == nullptr) {
                // First template
                root = templates.at(name);
                ts.prependTemplate(root.get());
            } else {
                // A parent template
                if (recursion_check.find(root->extendsTemplate()) != recursion_check.end()) {
                    throw CircularInheritanceException("Template '" + name + "' has circular dependency '" +
                                                       root->extendsTemplate() + "'");
                } else {
                    recursion_check[root->extendsTemplate()] = true;
                }

                root = templates.at(root->extendsTemplate());
                ts.prependTemplate(root.get());
            }
        } catch (std::out_of_range const&) {
            throw MissingTemplateException("Template '" + name + "' does not exist");
        }
    } while (root->extends());

    try {
        root->render(str, context, ts);
    } catch (std::exception& e) {
        throw TemplateException(e.what());
    }

    return str.str();
}

void Engine::addFilter(std::string const& name, Filter filter)
{
    filters[name] = std::move(filter);
}
