#include "redis_db.h"

#include <cpp_redis/cpp_redis>

#include "constants.h"
#include "redis_result.h"

using namespace bes::dbal::kv;

Redis::Redis(Context c) : KeyValueDb(std::move(c))
{
    connect();
}

void Redis::connect()
{
    auto log = [this](auto&& host, auto&& port, auto&& status) {
        logConnectStatus(host, port, status);
    };

    auto const& ctx = getContext();

    // Common connection config options
    uint32_t timeout = std::stoi(ctx.getOr(redis::CFG_CON_TIMEOUT, redis::DEFAULT_CON_TIMEOUT));
    int32_t max_connects = std::stoi(ctx.getOr(redis::CFG_MAX_RECONNECTS, redis::DEFAULT_MAX_RECONNECTS));
    uint32_t backoff = std::stoi(ctx.getOr(redis::CFG_CON_TIMEOUT, redis::DEFAULT_CON_TIMEOUT));

    if (ctx.hasParameter(redis::CFG_SENTINEL_SVC)) {
        // Sentinel connection
        try {
            client.connect(ctx.getParameter(redis::CFG_SENTINEL_SVC), log, timeout, max_connects, backoff);
        } catch (std::exception const& e) {
            throw DbalException(
                "Unable to connect to Redis Sentinel at " + ctx.getParameter(redis::CFG_SENTINEL_SVC) + ": " +
                e.what());
        }
    } else if (ctx.hasParameter(redis::CFG_HOSTNAME)) {
        // Direct server connection
        size_t port = std::stoi(ctx.getOr(redis::CFG_PORT, redis::DEFAULT_PORT));
        try {
            client.connect(ctx.getParameter(redis::CFG_HOSTNAME), port, log, timeout, max_connects, backoff);
        } catch (std::exception const& e) {
            throw DbalException(
                "Unable to connect to Redis server at " + ctx.getParameter(redis::CFG_HOSTNAME) + ":" +
                std::to_string(port) + ": " + e.what());
        }
    }
}

Redis& Redis::logConnectStatus(std::string const& host, std::size_t port, cpp_redis::connect_state status)
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

SuccessFuture Redis::apply(std::string const& key, std::string const& value)
{
    auto f = client.set(key, value);
    client.commit();
    return createSuccessFuture(f.share(), key);
}

SuccessFuture Redis::apply(std::string const& key, bes::dbal::Int64 value)
{
    auto f = client.set(key, std::to_string(value));
    client.commit();
    return createSuccessFuture(f.share(), key);
}

SuccessFuture Redis::apply(std::string const& key, bes::dbal::Float64 value)
{
    auto f = client.set(key, std::to_string(value));
    client.commit();
    return createSuccessFuture(f.share(), key);
}

SuccessFuture Redis::applyNx(std::string const& key, std::string const& value)
{
    auto f = client.setnx(key, value);
    client.commit();
    return createSuccessFuture(f.share(), key);
}

SuccessFuture Redis::applyNx(std::string const& key, bes::dbal::Int64 value)
{
    auto f = client.setnx(key, std::to_string(value));
    client.commit();
    return createSuccessFuture(f.share(), key);
}

SuccessFuture Redis::applyNx(std::string const& key, bes::dbal::Float64 value)
{
    auto f = client.setnx(key, std::to_string(value));
    client.commit();
    return createSuccessFuture(f.share(), key);
}

ResultFuture Redis::retrieve(std::string const& key)
{
    auto f = client.get(key);
    client.commit();
    return createResultFuture(f.share(), key);
}

SuccessFuture Redis::remove(std::string const& key)
{
    auto f = client.del({key});
    client.commit();
    return createSuccessFuture(f.share(), key);
}

SuccessFuture Redis::truncate()
{
    auto f = client.flushdb();
    client.commit();
    return createSuccessFuture(f.share(), std::string());
}

SuccessFuture Redis::offset(std::string const& key, bes::dbal::Int64 offset)
{
    auto f = client.incrby(key, offset);
    client.commit();
    return createSuccessFuture(f.share(), key);
}

SuccessFuture Redis::offset(std::string const& key, bes::dbal::Float64 offset)
{
    auto f = client.incrbyfloat(key, offset);
    client.commit();
    return createSuccessFuture(f.share(), key);
}

ResultFuture Redis::ttl(std::string const& key) {}

SuccessFuture Redis::expire(std::string const& key, size_t ttl) {}

SuccessFuture Redis::persist(std::string const& key) {}

SuccessFuture Redis::createSuccessFuture(std::shared_future<cpp_redis::reply> f, std::string key)
{
    return SuccessFuture([f = std::move(f), key = std::move(key)]() mutable {
        f.wait();
        auto const& reply = f.get();
        if (reply.is_error()) {
            throw DbalException("Error operating on key '" + key + "': " + reply.error());
        } else {
            return reply.ok();
        }
    });
}

ResultFuture Redis::createResultFuture(std::shared_future<cpp_redis::reply> ftr, std::string key)
{
    return ResultFuture(std::make_shared<redis::RedisResult>(std::move(ftr), std::move(key)));
}

SuccessFuture Redis::beginTransaction()
{
    if (in_transaction.load(std::memory_order_consume)) {
        throw DbalException("Redis transaction already begun, cannot start new transaction");
    }

    in_transaction.store(true, std::memory_order_release);

    auto f = client.multi();
    client.commit();

    return createSuccessFuture(f.share(), std::string());
}

SuccessFuture Redis::commitTransaction()
{
    if (!in_transaction.load(std::memory_order_acquire)) {
        throw DbalException("Redis transaction NOT begun, cannot commit transaction");
    }

    auto f = client.exec();
    client.commit();

    in_transaction.store(false, std::memory_order_release);

    return createSuccessFuture(f.share(), std::string());
}

SuccessFuture Redis::discardTransaction() {
    if (!in_transaction.load(std::memory_order_acquire)) {
        throw DbalException("Redis batch operation NOT begun, cannot discard batch");
    }

    auto f = client.discard();
    client.commit();

    in_transaction.store(false, std::memory_order_release);

    return createSuccessFuture(f.share(), std::string());
}

