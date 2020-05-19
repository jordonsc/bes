#ifndef BES_WEB_SESSION_H
#define BES_WEB_SESSION_H

#include <any>
#include <string>
#include <unordered_map>

namespace bes::web {

struct SessionObject
{
    SessionObject(SessionObject const&) = default;
    SessionObject(SessionObject&&) = default;
    SessionObject& operator=(SessionObject const&) = default;
    SessionObject& operator=(SessionObject&&) = default;

    SessionObject(char const*);
    SessionObject(std::string const&);
    SessionObject(int32_t);
    SessionObject(int64_t);
    SessionObject(double);
    SessionObject(bool);

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
    explicit Session(std::string id);
    Session(Session const&) = default;
    Session(Session&&) = default;

    [[nodiscard]] std::string const& SessionId() const;
    void SetValue(std::string const& key, SessionObject const& data);
    [[nodiscard]] SessionObject const& GetValue(std::string const& key) const;
    [[nodiscard]] std::unordered_map<std::string, SessionObject> const& Map() const;

    [[nodiscard]] std::string const& GetString(std::string const& key) const;
    [[nodiscard]] int64_t GetInt(std::string const& key) const;
    [[nodiscard]] double GetDouble(std::string const& key) const;
    [[nodiscard]] bool GetBool(std::string const& key) const;

    [[nodiscard]] size_t Size() const;
    [[nodiscard]] bool Empty() const;
    [[nodiscard]] bool HasItem(std::string const& key) const;

   protected:
    std::string session_id;
    std::unordered_map<std::string, SessionObject> map;
};

}  // namespace bes::web

#endif
