#include "context.h"

using namespace bes::dbal;

bool Context::HasParameter(std::string const& key) const
{
    return params.find(key) != params.end();
}

std::string const& Context::GetParameter(std::string const& key) const
{
    return params.at(key);
}

std::string const& Context::GetOr(std::string const& key, std::string const& or_value) const
{
    auto const& it = params.find(key);
    if (it != params.end()) {
        return it->second;
    } else {
        return or_value;
    }
}

void Context::SetParameter(std::string const& key, std::string value)
{
    params[key] = std::move(value);
}
