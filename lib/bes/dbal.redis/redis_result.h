#pragma once

#include <bes/dbal.h>

#include <cpp_redis/cpp_redis>
#include <future>

namespace bes::dbal::kv::redis {

using ResultInterface = bes::dbal::kv::ResultInterface;

class RedisResult : public ResultInterface
{
   public:
    using DispatchSig = std::function<void()>;

    RedisResult() = delete;

    /**
     * @param future Future from the Redis client itself
     * @param key    Redis key in question (for logging), can be blank
     * @param d      Callback to ask the client to send any unsent commands to the server
     */
    explicit RedisResult(std::shared_future<cpp_redis::reply> future, std::string key, DispatchSig d)
        : reply_future(std::move(future)),
          key(std::move(key)),
          dispatchFn(std::move(d))
    {}

    void wait() override
    {
        dispatchFn();
        reply_future.wait();
        reply = reply_future.get();
        if (!reply.ok()) {
            throw DbalException(reply.error());
        } else if (reply.is_null()) {
            throw DoesNotExistException("Key '" + key + "' does not exist");
        }
    }

    Text getString() override
    {
        wait();
        if (reply.is_string()) {
            return reply.as_string();
        } else {
            throw BadDataType("Data for key '" + key + "' is not a string");
        }
    }

    Int64 getInt() override
    {
        wait();
        if (reply.is_string()) {
            try {
                return std::stol(reply.as_string());
            } catch (std::invalid_argument const&) {
                throw BadDataType("Data for key '" + key + "' is not an integer");
            }
        } else {
            throw BadDataType("Data for key '" + key + "' cannot be represented as a string for integer conversion");
        }
    }

    Float64 getFloat() override
    {
        wait();
        if (reply.is_string()) {
            try {
                return std::stod(reply.as_string());
            } catch (std::invalid_argument const&) {
                throw BadDataType("Data for key '" + key + "' is not a floating point number");
            }
        } else {
            throw BadDataType("Data for key '" + key + "' cannot be represented as a string for float conversion");
        }
    }

   protected:
    cpp_redis::reply reply;
    std::shared_future<cpp_redis::reply> reply_future;
    std::string key;
    DispatchSig dispatchFn;  // allows the client to send any unsent commands to the server
};

}  // namespace bes::dbal::kv::redis