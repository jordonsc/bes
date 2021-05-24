#include "redis_session_mgr.h"

#include <iostream>
#include <vector>

using namespace bes::web;

RedisSessionMgr::RedisSessionMgr(bes::net::Address addr, uint32_t timeout_ms)
    : server(std::move(addr)), connect_timeout(timeout_ms)
{
    connect();
}

RedisSessionMgr::RedisSessionMgr(std::vector<bes::net::Address> const& sentinels, std::string sentinel_svc,
                                 uint32_t timeout_ms)
    : server(bes::net::Address("", 0)), sentinel_svc_name(std::move(sentinel_svc)), connect_timeout(timeout_ms)
{
    for (auto const& addr : sentinels) {
        client.add_sentinel(addr.hasIp6Addr() ? addr.ip6Addr() : addr.ip4Addr(), addr.port());
    }

    connect();
}

RedisSessionMgr* RedisSessionMgr::fromConfig(bes::Config const& config)
{
    auto timeout = config.getOr<uint32_t>(250, "web", "sessions", "timeout");
    auto sentinel_name = config.getOr<std::string>("", "web", "sessions", "redis", "sentinel-name");

    if (!sentinel_name.empty()) {
        // We've been given a sentinel service name, now check to see we've got at least one sentinel server, too -
        auto node = config.get<YAML::Node>("web", "sessions", "redis", "sentinels");
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

    bes::net::Address adr(config.getOr<std::string>("", "web", "sessions", "redis", "host"),
                          config.getOr<uint32_t>(6379, "web", "sessions", "redis", "port"));

    if (!adr.hasIp4Addr()) {
        throw WebException("No host or sentinels found in configuration for Redis session manager");
    }

    BES_LOG(DEBUG) << "Creating Redis session manager on server " << adr.ip4Addr() << ":" << adr.port();
    return new RedisSessionMgr(adr, timeout);
}

void RedisSessionMgr::connect()
{
    auto log = [this](auto&& host, auto&& port, auto&& status) {
        logConnectStatus(host, port, status);
    };

    try {
        if (server.port() == 0) {
            client.connect(sentinel_svc_name, log, connect_timeout);
        } else {
            client.connect(server.hasIp6Addr() ? server.ip6Addr() : server.ip4Addr(), server.port(), log, 250);
        }
    } catch (std::exception& e) {
        BES_LOG(ERROR) << "Unable to open connection to Redis server on " << server.ip4Addr() << ":" << server.port();
        BES_LOG(ERROR) << e.what();
        throw WebException("Unable to connect to Redis server");
    }
}

RedisSessionMgr& RedisSessionMgr::logConnectStatus(std::string const& host, std::size_t port,
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

Session RedisSessionMgr::createSession(std::string const& ns)
{
    bool ok = false;
    std::string id;
    uint16_t attempts = 0;

    do {
        id = SessionInterface::generateSessionKey(ns);

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

Session RedisSessionMgr::getSession(std::string const& session_id)
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
                    session.setValue(key, SessionObject(value[1] == '1'));
                    break;
                case 'S':
                    session.setValue(key, SessionObject(value.substr(1)));
                    break;
                case 'I':
                    session.setValue(key, SessionObject(atol(value.substr(1).c_str())));
                    break;
                case 'D':
                    session.setValue(key, SessionObject(atof(value.substr(1).c_str())));
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

void RedisSessionMgr::setSessionTtl(uint64_t ttl)
{
    session_ttl = ttl;
}

void RedisSessionMgr::persistSession(Session const& session)
{
    std::string id("session:");

    if (session.sessionId().empty()) {
        throw WebException("Attempting to persist a null session");
    } else {
        id += session.sessionId();
    }

    std::vector<std::pair<std::string, std::string>> hash;
    for (auto const& it : session.getMap()) {
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
