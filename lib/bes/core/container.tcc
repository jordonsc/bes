#pragma once

#include <any>
#include <memory>
#include <shared_mutex>
#include <unordered_map>

#include "exception.h"

namespace bes {

class Container
{
   public:
    template <class T>
    Container& add(std::string const& key, std::shared_ptr<T> ptr);

    template <class T, class... Args>
    Container& emplace(std::string const& key, Args&&... args);

    bool exists(std::string const& key) const;

    template <class T>
    std::shared_ptr<T> get(std::string const& key) const;

    void remove(std::string const& key);

   private:
    std::unordered_map<std::string, std::any> bucket;
    mutable std::shared_mutex mutex;
};

template <class T>
inline Container& Container::add(std::string const& key, std::shared_ptr<T> ptr)
{
    if (exists(key)) {
        throw KeyExistsException("Key '" + key + "' already exists in container");
    }

    std::lock_guard<std::shared_mutex> lock(mutex);
    bucket[key] = ptr;

    return *this;
}

template <class T, class... Args>
inline Container& Container::emplace(std::string const& key, Args&&... args)
{
    if (exists(key)) {
        throw KeyExistsException("Key '" + key + "' already exists in container");
    }

    std::lock_guard<std::shared_mutex> lock(mutex);
    bucket[key] = std::make_shared<T>(std::forward<Args>(args)...);

    return *this;
}

inline bool Container::exists(std::string const& key) const
{
    std::shared_lock<std::shared_mutex> lock(mutex);
    return bucket.find(key) != bucket.end();
}

template <class T>
inline std::shared_ptr<T> Container::get(std::string const& key) const
{
    std::shared_lock<std::shared_mutex> lock(mutex);

    auto it = bucket.find(key);
    if (it == bucket.end()) {
        throw KeyNotFoundException("Key '" + key + "' not found in container");
    }

    return std::any_cast<std::shared_ptr<T>>(it->second);
}

inline void Container::remove(std::string const& key)
{
    std::lock_guard<std::shared_mutex> lock(mutex);
    try {
        bucket.erase(key);
    } catch (...) {
        throw KeyNotFoundException("Key '" + key + "' not found in container");
    }
}

}  // namespace bes
