#ifndef BES_WEB_SESSION_H
#define BES_WEB_SESSION_H

#include <any>
#include <string>
#include <unordered_map>

namespace bes::web {

struct SessionObject
{
    SessionObject(char const*);
    SessionObject(std::string const&);
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

    std::any const data;
    ObjectType const data_type;
};

class Session
{
   public:
    explicit Session(std::string id);
    Session(Session const&) = default;
    Session(Session&&) = default;

    std::string const& SessionId() const;
    void SetValue(std::string const& key, SessionObject const& data);
    SessionObject const& GetValue(std::string const& key) const;
    std::unordered_map<std::string, SessionObject> const& Map() const;

    std::string const& GetString(std::string const& key) const;
    int64_t GetInt(std::string const& key) const;
    double GetDouble(std::string const& key) const;
    bool GetBool(std::string const& key) const;

   protected:
    std::string session_id;
    std::unordered_map<std::string, SessionObject> map;
};

}  // namespace bes::web

#endif
