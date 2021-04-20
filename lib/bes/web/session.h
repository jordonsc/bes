#pragma once

#include <any>
#include <string>
#include <unordered_map>

#include "exception.h"

namespace bes::web {

struct SessionObject
{
    explicit SessionObject(char const*);
    explicit SessionObject(std::string);
    explicit SessionObject(int64_t);
    explicit SessionObject(double);
    explicit SessionObject(bool);

    enum class ObjectType : char
    {
        STRING,
        INT64,
        DOUBLE,
        BOOL,
    };

    std::any data;
    ObjectType data_type;
};

class Session
{
   public:
    Session() = default;
    explicit Session(std::string id);

    [[nodiscard]] std::string const& sessionId() const;

    void setValue(std::string const& key, SessionObject data);
    void setValue(std::string const& key, std::string data);
    void setValue(std::string const& key, char const* data);
    void setValue(std::string const& key, int64_t data);
    void setValue(std::string const& key, double data);
    void setValue(std::string const& key, bool data);

    [[nodiscard]] SessionObject const& getValue(std::string const& key) const;
    [[nodiscard]] std::unordered_map<std::string, SessionObject> const& getMap() const;

    [[nodiscard]] std::string const& getString(std::string const& key) const;
    [[nodiscard]] int64_t getInt(std::string const& key) const;
    [[nodiscard]] double getDouble(std::string const& key) const;
    [[nodiscard]] bool getBool(std::string const& key) const;

    [[nodiscard]] size_t size() const;
    [[nodiscard]] bool empty() const;
    [[nodiscard]] bool hasItem(std::string const& key) const;

   protected:
    std::string session_id;
    std::unordered_map<std::string, SessionObject> map;
};

}  // namespace bes::web
