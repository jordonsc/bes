#include "context.h"

using namespace bes::templating::data;

Context::Context()
{
    increaseStack();
}

Context& Context::increaseStack()
{
    std::unique_lock<std::shared_mutex> lock(value_mutex);

    data.emplace_back();
    return *this;
}

Context& Context::decreaseStack()
{
    if (data.size() == 1) {
        throw TemplateException("Cannot decrease a context stack to zero");
    }

    std::unique_lock<std::shared_mutex> lock(value_mutex);

    data.pop_back();
    return *this;
}

Context& Context::setValue(std::string const& key, std::shared_ptr<ShellInterface> item)
{
    std::unique_lock<std::shared_mutex> lock(value_mutex);

    data.back().insert_or_assign(key, item);
    return *this;
}

std::shared_ptr<ShellInterface> const& Context::getValue(std::string const& key)
{
    std::shared_lock<std::shared_mutex> lock(value_mutex);

    for (auto it = data.rbegin(); it != data.rend(); ++it) {
        auto const& cm = it->find(key);
        if (cm != it->end()) {
            return cm->second;
        }
    }

    throw MissingContextException("Context item '" + key + "' does not exist");
}

void Context::addMacro(std::string const& key, node::Node const* node)
{
    macros[key] = node;
}

bool Context::hasMacro(std::string const& key) const
{
    return macros.find(key) != macros.end();
}

bes::templating::node::Node const* Context::getMacro(std::string const& key) const
{
    return macros.at(key);
}
