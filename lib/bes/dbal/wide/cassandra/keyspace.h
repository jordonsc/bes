#pragma once

#include <string>
#include <unordered_map>
#include <utility>

namespace bes::dbal::wide::cassandra {

enum class ReplicationStrategy : char
{
    SIMPLE,
    NETWORK_TOPOLOGY,
};

struct Keyspace final
{
    explicit Keyspace(std::string name, ReplicationStrategy rep_strategy = ReplicationStrategy::SIMPLE,
                      uint16_t rep_factor = 3, bool durable_writes = true)
        : name(std::move(name)),
          replication_strategy(rep_strategy),
          replication_factor(rep_factor),
          durable_writes(durable_writes)
    {}

    std::string name;
    ReplicationStrategy replication_strategy;
    uint16_t replication_factor;
    bool durable_writes;
    std::unordered_map<std::string, uint16_t> dc_replication;
};

}  // namespace bes::dbal::wide::cassandra

