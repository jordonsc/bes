#include "context.h"

using namespace bes::templating::data;

Context::Context()
{
    IncreaseStack();
}

Context& Context::IncreaseStack()
{
    std::unique_lock<std::shared_mutex> lock(value_mutex);

    data.emplace_back();
    return *this;
}

Context& Context::DecreaseStack()
{
    if (data.size() == 1) {
        throw TemplateException("Cannot decrease a context stack to zero");
    }

    std::unique_lock<std::shared_mutex> lock(value_mutex);

    data.pop_back();
    return *this;
}

Context& Context::SetValue(std::string const& key, std::shared_ptr<ShellInterface> item)
{
    std::unique_lock<std::shared_mutex> lock(value_mutex);

    data.back().insert_or_assign(key, item);
    return *this;
}

std::shared_ptr<ShellInterface> const& Context::GetValue(std::string const& key)
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
