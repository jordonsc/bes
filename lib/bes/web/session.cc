#include "session.h"

#include <any>
#include <utility>

using namespace bes::web;

SessionObject::SessionObject(char const* item) : data(std::string(item)), data_type(ObjectType::STRING) {}
SessionObject::SessionObject(std::string item) : data(std::move(item)), data_type(ObjectType::STRING) {}
SessionObject::SessionObject(int64_t item) : data(item), data_type(ObjectType::INT64) {}
SessionObject::SessionObject(double item) : data(item), data_type(ObjectType::DOUBLE) {}
SessionObject::SessionObject(bool item) : data(item), data_type(ObjectType::BOOL) {}

Session::Session(std::string id) : session_id(std::move(id)) {}

std::string const& Session::sessionId() const
{
    return session_id;
};

void Session::setValue(std::string const& key, SessionObject data)
{
    map.insert_or_assign(key, std::move(data));
}

void Session::setValue(std::string const& key, std::string data)
{
    setValue(key, SessionObject(std::move(data)));
}

void Session::setValue(std::string const& key, const char* data)
{
    setValue(key, SessionObject(data));
}

void Session::setValue(std::string const& key, int64_t data)
{
    setValue(key, SessionObject(data));
}

void Session::setValue(std::string const& key, double data)
{
    setValue(key, SessionObject(data));
}

void Session::setValue(std::string const& key, bool data)
{
    setValue(key, SessionObject(data));
}

SessionObject const& Session::getValue(std::string const& key) const
{
    try {
        return map.at(key);
    } catch (...) {
        throw SessionIndexError("Session key '" + key + "' does not exist");
    }
};

std::string const& Session::getString(std::string const& key) const
{
    try {
        return std::any_cast<std::string const&>(map.at(key).data);
    } catch (...) {
        throw SessionIndexError("Session key '" + key + "' does not exist");
    }
}

int64_t Session::getInt(std::string const& key) const
{
    try {
        return std::any_cast<int64_t>(map.at(key).data);
    } catch (...) {
        throw SessionIndexError("Session key '" + key + "' does not exist");
    }
}

double Session::getDouble(std::string const& key) const
{
    try {
        return std::any_cast<double>(map.at(key).data);
    } catch (...) {
        throw SessionIndexError("Session key '" + key + "' does not exist");
    }
}

bool Session::getBool(std::string const& key) const
{
    try {
        return std::any_cast<bool>(map.at(key).data);
    } catch (...) {
        throw SessionIndexError("Session key '" + key + "' does not exist");
    }
}

std::unordered_map<std::string, SessionObject> const& Session::getMap() const
{
    return map;
}

size_t Session::size() const
{
    return map.size();
}

bool Session::empty() const
{
    return map.empty();
}

bool Session::hasItem(std::string const& key) const
{
    return map.find(key) != map.end();
}
