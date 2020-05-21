#include "session.h"

#include <any>
#include <utility>

using namespace bes::web;

SessionObject::SessionObject(char const* item) : data(std::string(item)), data_type(ObjectType::STRING) {}
SessionObject::SessionObject(std::string const& item) : data(item), data_type(ObjectType::STRING) {}
SessionObject::SessionObject(int32_t item) : data(int64_t(item)), data_type(ObjectType::INT64) {}
SessionObject::SessionObject(int64_t item) : data(item), data_type(ObjectType::INT64) {}
SessionObject::SessionObject(double item) : data(item), data_type(ObjectType::DOUBLE) {}
SessionObject::SessionObject(bool item) : data(item), data_type(ObjectType::BOOL) {}

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
    try {
        return map.at(key);
    } catch (...) {
        throw SessionIndexError("Session key '" + key + "' does not exist");
    }
};

std::string const& Session::GetString(std::string const& key) const
{
    try {
        return std::any_cast<std::string const&>(map.at(key).data);
    } catch (...) {
        throw SessionIndexError("Session key '" + key + "' does not exist");
    }
}

int64_t Session::GetInt(std::string const& key) const
{
    try {
        return std::any_cast<int64_t>(map.at(key).data);
    } catch (...) {
        throw SessionIndexError("Session key '" + key + "' does not exist");
    }
}

double Session::GetDouble(std::string const& key) const
{
    try {
        return std::any_cast<double>(map.at(key).data);
    } catch (...) {
        throw SessionIndexError("Session key '" + key + "' does not exist");
    }
}

bool Session::GetBool(std::string const& key) const
{
    try {
        return std::any_cast<bool>(map.at(key).data);
    } catch (...) {
        throw SessionIndexError("Session key '" + key + "' does not exist");
    }
}

std::unordered_map<std::string, SessionObject> const& Session::Map() const
{
    return map;
}

size_t Session::Size() const
{
    return map.size();
}

bool Session::Empty() const
{
    return map.empty();
}

bool Session::HasItem(std::string const& key) const
{
    return map.find(key) != map.end();
}
