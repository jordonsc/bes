#pragma once

#include <bes/dbal.h>

#include <atomic>

#include "redis_result.h"

namespace bes::dbal::kv {

class Redis : public KeyValueDb
{
   public:
    Redis() = delete;
    explicit Redis(Context c);

    inline bool isConnected()
    {
        return client.is_connected();
    }

    SuccessFuture apply(std::string const& key, std::string const& value) override;
    SuccessFuture apply(std::string const& key, Int64 value) override;
    SuccessFuture apply(std::string const& key, Float64 value) override;

    SuccessFuture applyNx(std::string const& key, std::string const& value) override;
    SuccessFuture applyNx(std::string const& key, Int64 value) override;
    SuccessFuture applyNx(std::string const& key, Float64 value) override;

    ResultFuture retrieve(std::string const& key) override;

    SuccessFuture remove(std::string const& key) override;
    SuccessFuture truncate() override;

    // Increment/decrement
    SuccessFuture offset(std::string const& key, Int64 offset) override;
    SuccessFuture offset(std::string const& key, Float64 offset) override;

    // TTL operations
    ResultFuture ttl(std::string const& key) override;
    SuccessFuture expire(std::string const& key, size_t ttl) override;
    SuccessFuture persist(std::string const& key) override;

    // Transactions
    SuccessFuture beginTransaction() override;
    SuccessFuture commitTransaction() override;
    SuccessFuture discardTransaction() override;

   protected:
    std::atomic<bool> in_transaction = false;
    cpp_redis::client client;
    static SuccessFuture createSuccessFuture(std::shared_future<cpp_redis::reply> f, std::string key);
    static ResultFuture createResultFuture(std::shared_future<cpp_redis::reply> f, std::string key);

    void connect();
    Redis& logConnectStatus(std::string const& host, std::size_t port, cpp_redis::connect_state status);
};

}  // namespace bes::dbal::kv
