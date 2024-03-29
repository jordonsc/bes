#pragma once

#include "../context_db.h"
#include "../datatype.h"
#include "result_future.h"
#include "success_future.h"

using bes::dbal::Context;

namespace bes::dbal::kv {

class KeyValueDb : public ContextualDatabase
{
   public:
    KeyValueDb() = delete;
    explicit KeyValueDb(Context c) : ContextualDatabase(std::move(c)) {}

    // Standard key operations
    /**
     * Write an object to the database, overwriting any previous data.
     */
    virtual SuccessFuture apply(std::string const& key, std::string const& value) = 0;
    virtual SuccessFuture apply(std::string const& key, Int64 value) = 0;
    virtual SuccessFuture apply(std::string const& key, Float64 value) = 0;

    /**
     * Write an object to the database, will NOT overwrite any existing records.
     */
    virtual SuccessFuture applyNx(std::string const& key, std::string const& value) = 0;
    virtual SuccessFuture applyNx(std::string const& key, Int64 value) = 0;
    virtual SuccessFuture applyNx(std::string const& key, Float64 value) = 0;

    /**
     * Retrieve an object.
     *
     * @throws DoesNotExistException if the record does not exist.
     */
    virtual ResultFuture retrieve(std::string const& key) = 0;

    /**
     * Delete an object, does nothing if the record does not exist.
     */
    virtual SuccessFuture remove(std::string const& key) = 0;

    /**
     * Truncate the entire database.
     */
    virtual SuccessFuture truncate() = 0;

    // Number operations (incr/decr)
    /**
     * Increment or decrement a value by its numeric form. If the key does not exist, a value of 0 will be used.
     */
    virtual SuccessFuture offset(std::string const& key, Int64 offset) = 0;
    virtual SuccessFuture offset(std::string const& key, Float64 offset) = 0;

    // TTL operations
    /**
     * Return the remaining time-to-live, in seconds, for this key.
     */
    virtual ResultFuture ttl(std::string const& key) = 0;

    /**
     * Set the expiration time (from now) in seconds.
     */
    virtual SuccessFuture expire(std::string const& key, size_t ttl) = 0;

    /**
     * Remove the TTL from a key, allowing it to live indefinitely.
     */
    virtual SuccessFuture persist(std::string const& key) = 0;

    // Transactions
    /**
     * Begin a transaction.
     *
     * All successive calls will be sent to the server but not committed until `commitTransaction()` is called. If you
     * have already begun a transaction and not committed it, a LogicException will be thrown.
     */
    virtual SuccessFuture beginTransaction() = 0;

    /**
     * Commit and execute a transaction.
     *
     * All calls raised since `beginTransaction()` will now be executed on the server. If a transaction has not been
     * started a LogicException will be thrown.
     */
    virtual SuccessFuture commitTransaction() = 0;

    /**
     * Discard a transaction.
     *
     * All calls raised since `beginTransaction()` will be discarded. If a transaction has not been started,
     * a LogicException will be thrown.
     */
    virtual SuccessFuture discardTransaction() = 0;

    // Pipelining
    /**
     * Send all queued commands to the server.
     *
     * If no commands are queued, this will do nothing. Returns immediately, must be asynchronous.
     */
    virtual void dispatch() = 0;
};

}  // namespace bes::dbal::kv
