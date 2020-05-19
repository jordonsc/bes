#include "session.h"

#include <any>
#include <utility>

using namespace bes::web;

Session::Session(std::string id) : session_id(std::move(id)) {}

std::string const& Session::SessionId() const
{
    return session_id;
};

void Session::SetValue(std::string const& key, SessionObject const& data)
{
    map.insert_or_assign(key, data);
}

SessionObject const& Session::GetValue(std::string const& key) const
{
    return map.at(key);
};

std::string const& Session::GetString(std::string const& key) const
{
    return std::any_cast<std::string const&>(map.at(key).data);
};

int64_t Session::GetInt(std::string const& key) const
{
    return std::any_cast<int64_t>(map.at(key).data);
}

double Session::GetDouble(std::string const& key) const
{
    return std::any_cast<double>(map.at(key).data);
};

bool Session::GetBool(std::string const& key) const
{
    return std::any_cast<bool>(map.at(key).data);
}

std::unordered_map<std::string, SessionObject> const& Session::Map() const
{
    return map;
}

SessionObject::SessionObject(char const* item) : data(item), data_type(ObjectType::STRING) {}

SessionObject::SessionObject(std::string const& item) : data(item), data_type(ObjectType::STRING) {}

SessionObject::SessionObject(int64_t item) : data(item), data_type(ObjectType::INT64) {}

SessionObject::SessionObject(double item) : data(item), data_type(ObjectType::DOUBLE) {}

SessionObject::SessionObject(bool item) : data(item), data_type(ObjectType::BOOL) {}
