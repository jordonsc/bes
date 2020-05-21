#include "engine.h"

#include <utility>

using namespace bes::templating;

Engine::Engine()
{
    AddFilter("trim", [](std::string& txt) -> void {
        Text::Trim(txt);
    });

    AddFilter("ltrim", [](std::string& txt) -> void {
        Text::TrimFront(txt);
    });

    AddFilter("rtrim", [](std::string& txt) -> void {
        Text::TrimBack(txt);
    });

    AddFilter("nl2br", [](std::string& txt) -> void {
        Text::Nl2Br(txt);
    });

    AddFilter("nl2p", [](std::string& txt) -> void {
        Text::Nl2P(txt);
    });
}

void Engine::LoadFile(std::string const& name, std::string const& filename)
{
    std::unique_lock<std::shared_mutex> lock;

    if (search_path.Empty()) {
        search_path.AppendSearchPath(".");
    }

    std::string file_path = search_path.FindInPath(filename);

    std::ifstream str(file_path);
    std::stringstream buffer;
    buffer << str.rdbuf();

    auto ptr = std::make_shared<node::RootNode>(name);
    parser.Parse(*(ptr.get()), buffer.str());

    ptr->filters = &filters;
    templates.insert_or_assign(name, ptr);

    BES_LOG(DEBUG) << "Loaded template '" << name << "' from file '" << file_path << "'";
}

void Engine::LoadString(std::string const& name, std::string const& data)
{
    std::unique_lock<std::shared_mutex> lock;

    auto ptr = std::make_shared<node::RootNode>(name);
    parser.Parse(*(ptr.get()), data);

    ptr->filters = &filters;
    templates.insert_or_assign(name, ptr);

    BES_LOG(DEBUG) << "Loaded template '" << name << "' from memory";
}

std::string Engine::Render(std::string const& name, data::Context& context)
{
    std::shared_lock<std::shared_mutex> lock;

    std::ostringstream str;
    std::shared_ptr<node::RootNode const> root;
    std::unordered_map<std::string, bool> recursion_check;
    recursion_check[name] = true;

    // Build a template stack, used during node rendering when encountering child templates or includes
    data::TemplateStack ts(this);
    ts.PrependTemplate(nullptr);

    do {
        try {
            if (root == nullptr) {
                // First template
                root = templates.at(name);
                ts.PrependTemplate(root.get());
            } else {
                // A parent template
                if (recursion_check.find(root->ExtendsTemplate()) != recursion_check.end()) {
                    throw CircularInheritanceException("Template '" + name + "' has circular dependency '" +
                                                       root->ExtendsTemplate() + "'");
                } else {
                    recursion_check[root->ExtendsTemplate()] = true;
                }

                root = templates.at(root->ExtendsTemplate());
                ts.PrependTemplate(root.get());
            }
        } catch (std::out_of_range const&) {
            throw MissingTemplateException("Template '" + name + "' does not exist");
        }
    } while (root->Extends());

    try {
        root->Render(str, context, ts);
    } catch (std::exception& e) {
        throw TemplateException(e.what());
    }

    return str.str();
}

void Engine::AddFilter(std::string const& name, Filter filter)
{
    filters[name] = std::move(filter);
}