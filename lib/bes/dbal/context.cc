#include "context.h"

#include <utility>

#include "exception.h"

using namespace bes::dbal;

bool Context::hasParameter(std::string const& key) const
{
    return params.find(key) != params.end();
}

std::string const& Context::getParameter(std::string const& key) const
{
    try {
        return params.at(key);
    } catch (std::exception const&) {
        throw OutOfRangeException("Key '" + key + "' does not exists in context");
    }
}

std::string const& Context::getOr(std::string const& key, std::string const& or_value) const
{
    auto const& it = params.find(key);
    if (it != params.end()) {
        return it->second;
    } else {
        return or_value;
    }
}

void Context::setParameter(std::string const& key, std::string value)
{
    params[key] = std::move(value);
}

Context::Context(std::unordered_map<std::string, std::string> params) : params(std::move(params)) {}
