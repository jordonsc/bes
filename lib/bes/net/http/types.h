#pragma once

#include <memory>
#include <shared_mutex>

#include "option.h"

namespace bes::net::http {

using opt_mutex_t = std::shared_mutex;
using opt_rlock_t = std::shared_lock<opt_mutex_t>;
using opt_wlock_t = std::unique_lock<opt_mutex_t>;
using opt_ptr = std::shared_ptr<Option>;

}  // namespace bes::net::http
