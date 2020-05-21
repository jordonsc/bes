#include "redis_session_mgr.h"

#include <functional>
#include <iostream>
#include <vector>

using namespace bes::web;

RedisSessionMgr::RedisSessionMgr(bes::net::Address addr, uint32_t timeout_ms)
    : server(std::move(addr)), connect_timeout(timeout_ms)
{
    Connect();
}

RedisSessionMgr::RedisSessionMgr(std::vector<bes::net::Address> const& sentinels, std::string sentinel_svc,
                                 uint32_t timeout_ms)
    : server(bes::net::Address("", 0)), sentinel_svc_name(std::move(sentinel_svc)), connect_timeout(timeout_ms)
{
    for (auto const& addr : sentinels) {
        client.add_sentinel(addr.HasIp6Addr() ? addr.Ip6Addr() : addr.Ip4Addr(), addr.Port());
    }

    Connect();
}

RedisSessionMgr* RedisSessionMgr::FromConfig(bes::Config const& config)
{
    auto timeout = config.GetOr<uint32_t>(250, "web", "sessions", "timeout");
    auto sentinel_name = config.GetOr<std::string>("", "web", "sessions", "redis", "sentinel-name");

    if (!sentinel_name.empty()) {
        // We've been given a sentinel service name, now check to see we've got at least one sentinel server, too -
        auto node = config.Get<YAML::Node>("web", "sessions", "redis", "sentinels");
        if (node.IsDefined() && node.IsSequence() && node.size()) {
            std::vector<bes::net::Address> sentinels;

            // Looks like we've got sentinels, add them to a vector
            for (auto const& sentinel : node) {
                auto host_node = sentinel["host"];
                if (host_node.IsDefined() && host_node.IsScalar()) {
                    auto port_node = sentinel["port"];
                    if (port_node.IsDefined() && port_node.IsScalar()) {
                        sentinels.emplace_back(host_node.as<std::string>(), port_node.as<uint32_t>());
                    } else {
                        sentinels.emplace_back(host_node.as<std::string>(), 6379);
                    }

                } else {
                    throw WebException("Sentinel must have 'host' key in configuration");
                }
            }

            if (!sentinels.empty()) {
                // We've now got everything we need for a sentinel-based config, build and return
                BES_LOG(DEBUG) << "Creating Redis session manager with " << sentinels.size() << " sentinels";
                return new RedisSessionMgr(sentinels, sentinel_name, timeout);
            }
        }
    }

    bes::net::Address adr(config.GetOr<std::string>("", "web", "sessions", "redis", "host"),
                          config.GetOr<uint32_t>(6379, "web", "sessions", "redis", "port"));

    if (!adr.HasIp4Addr()) {
        throw WebException("No host or sentinels found in configuration for Redis session manager");
    }

    BES_LOG(DEBUG) << "Creating Redis session manager on server " << adr.Ip4Addr() << ":" << adr.Port();
    return new RedisSessionMgr(adr, timeout);
}

void RedisSessionMgr::Connect()
{
    using namespace std::placeholders;
    auto log = std::bind(&RedisSessionMgr::LogConnectStatus, this, _1, _2, _3);

    if (server.Port() == 0) {
        client.connect(sentinel_svc_name, log, connect_timeout);
    } else {
        client.connect(server.HasIp6Addr() ? server.Ip6Addr() : server.Ip4Addr(), server.Port(), log, 250);
    }
}

RedisSessionMgr& RedisSessionMgr::LogConnectStatus(std::string const& host, std::size_t port,
                                                   cpp_redis::connect_state status)
{
    switch (status) {
        case cpp_redis::connect_state::dropped:
            BES_LOG(INFO) << "Redis disconnected from " << host << ":" << port;
            break;
        case cpp_redis::connect_state::failed:
            BES_LOG(ERROR) << "Redis connection failed to " << host << ":" << port;
            break;
        case cpp_redis::connect_state::ok:
            BES_LOG(INFO) << "Redis connected to " << host << ":" << port;
            break;
        case cpp_redis::connect_state::start:
            BES_LOG(TRACE) << "Redis start: " << host << ":" << port;
            break;
        case cpp_redis::connect_state::lookup_failed:
            BES_LOG(ERROR) << "Lookup failure: " << host << ":" << port;
            break;
        case cpp_redis::connect_state::sleeping:
            BES_LOG(TRACE) << "Sleeping: " << host << ":" << port;
            break;
        case cpp_redis::connect_state::stopped:
            BES_LOG(INFO) << "Redis connection stopped: " << host << ":" << port;
            break;
        default:
            BES_LOG(ERROR) << "Unknown redis state: " << host << ":" << port;
            break;
    }

    return *this;
}

Session RedisSessionMgr::CreateSession()
{
    bool ok = false;
    std::string id;
    uint16_t attempts = 0;

    do {
        id = SessionInterface::GenerateSessionKey();

        // Commit a blank value to the DB to ensure there is no collision
        client.setnx("session:" + id, "-", [&ok](cpp_redis::reply& reply) {
            ok = reply.ok();
        });
        client.sync_commit();

        if (++attempts == 50) {
            throw WebException("Too many attempts creating unique session ID");
        }
    } while (!ok);

    return Session(id);
}

Session RedisSessionMgr::GetSession(std::string const& session_id)
{
    bes::web::Session session(session_id);

    std::string id("session:");
    id += session_id;
    bool valid = true;

    client.exists({id}, [&valid](cpp_redis::reply& reply) {
        if (!reply.ok() || !reply.is_integer() || reply.as_integer() != 1) {
            valid = false;
        }
    });

    client.hgetall(id, [&valid, &session, &session_id](cpp_redis::reply& reply) {
        if (!reply.ok() || !reply.is_array()) {
            valid = false;
            return;
        }

        auto const& arr = reply.as_array();
        if (arr.size() % 2 != 0) {
            BES_LOG(WARNING) << "Session value size error for session '" << session_id << "': " << arr.size();
            valid = false;
            return;
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
    });

    client.sync_commit();

    if (!valid) {
        throw SessionNotExistsException("Session with ID '" + session_id + "' does not exist");
    }

    return session;
}

void RedisSessionMgr::SetSessionTtl(uint64_t ttl)
{
    session_ttl = ttl;
}

void RedisSessionMgr::PersistSession(Session const& session)
{
    std::string id("session:");

    if (session.SessionId().empty()) {
        throw WebException("Attempting to persist a null session");
    } else {
        id += session.SessionId();
    }

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

    if (session_ttl) {
        client.expire(id, session_ttl);
    }

    client.sync_commit();
}
