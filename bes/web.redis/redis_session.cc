#include "redis_session.h"

#include <iostream>

using namespace bes::web;

RedisSession::RedisSession(bes::net::Address addr) : server(std::move(addr))
{
    Connect();
}

void RedisSession::Connect()
{
    client.connect(server.HasIp6Addr() ? server.Ip6Addr() : server.Ip4Addr(), server.Port(),
                   [](std::string const& host, std::size_t port, cpp_redis::connect_state status) {
                       switch (status) {
                           case cpp_redis::connect_state::dropped:
                               BES_LOG(ERROR) << "Redis disconnected from " << host << ":" << port;
                               break;
                           case cpp_redis::connect_state::failed:
                               BES_LOG(ERROR) << "Redis connection failed to " << host << ":" << port;
                               break;
                           case cpp_redis::connect_state::ok:
                               BES_LOG(INFO) << "Redis connected to " << host << ":" << port;
                               break;
                           case cpp_redis::connect_state::start:
                               BES_LOG(DEBUG) << "Redis start: " << host << ":" << port;
                               break;
                           case cpp_redis::connect_state::lookup_failed:
                               BES_LOG(ERROR) << "Lookup failure: " << host << ":" << port;
                               break;
                           case cpp_redis::connect_state::sleeping:
                               BES_LOG(DEBUG) << "Sleeping: " << host << ":" << port;
                               break;
                           case cpp_redis::connect_state::stopped:
                               BES_LOG(NOTICE) << "Redis connection stopped: " << host << ":" << port;
                               break;
                           default:
                               BES_LOG(NOTICE) << "Unknown redis state: " << host << ":" << port;
                               break;
                       }
                   });
}

Session RedisSession::CreateSession()
{
    std::string id = "S";
    int64_t index = 0;

    client.incr("session.index", [&index](cpp_redis::reply& reply) {
        if (reply.ok() && reply.is_integer()) {
            index = reply.as_integer();
        } else {
            throw WebException("Session tables not configured correctly");
        }
    });
    client.sync_commit();

    // TODO: something cool here, thinking the current unix time in hex concatenated w/ index in hex form
    id += std::to_string(index);

    return Session(id);
}

Session RedisSession::GetSession(std::string const& session_id)
{
    bes::web::Session session(session_id);

    std::string id("session:");
    id += session_id;

    std::cout << "Getting session: " << id << std::endl;
    auto reply_f = client.hgetall(id);
    client.commit();

    auto reply = reply_f.get();

    if (reply.ok() && reply.is_array()) {
        auto const& arr = reply.as_array();
        if (arr.size() % 2 != 0) {
            BES_LOG(WARNING) << "Session value size error for session '" << session_id << "': " << arr.size();
            return session;
        }

        for (auto it = arr.begin(); it != arr.end(); ++it) {
            std::string const& key = it->as_string();
            ++it;
            std::string const& value = it->as_string();

            if (key.empty() || value.size() < 2) {
                continue;
            }

            char v_type = value[0];
            switch (v_type) {
                case 'B':
                    session.SetValue(key, value[1] == '1');
                    break;
                case 'S':
                    session.SetValue(key, value.substr(1));
                    break;
                case 'I':
                    session.SetValue(key, atol(value.substr(1).c_str()));
                    break;
                case 'D':
                    session.SetValue(key, atof(value.substr(1).c_str()));
                    break;
                default:
                    break;
            }
        }
    }

    return session;
}

void RedisSession::PersistSession(Session const& session, size_t ttl)
{
    std::string id("session:");
    id += session.SessionId();

    std::vector<std::pair<std::string, std::string>> hash;
    for (auto const& it : session.Map()) {
        switch (it.second.data_type) {
            case SessionObject::ObjectType::DOUBLE:
                hash.emplace_back(it.first, "D" + std::to_string(std::any_cast<double>(it.second.data)));
                break;
            case SessionObject::ObjectType::STRING:
                hash.emplace_back(it.first, "S" + std::any_cast<std::string>(it.second.data));
                break;
            case SessionObject::ObjectType::INT64:
                hash.emplace_back(it.first, "I" + std::to_string(std::any_cast<int64_t>(it.second.data)));
                break;
            case SessionObject::ObjectType::BOOL:
                hash.emplace_back(it.first, "B" + std::to_string(std::any_cast<bool>(it.second.data)));
                break;
            default:
                throw WebException("Unknown session object type: " + it.first);
        }
    }

    client.del({id});
    client.hmset(id, hash);
    if (ttl) {
        client.expire(id, ttl);
    }
    client.sync_commit();
}
