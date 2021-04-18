#pragma once

#include <memory>
#include <utility>

#include "result.h"

namespace bes::dbal::wide {

class ResultFuture
{
   public:
    explicit ResultFuture(std::shared_ptr<bes::dbal::wide::Result> result) : result(std::move(result)) {}

    [[nodiscard]] Result* operator->() const
    {
        return result.get();
    }

    [[nodiscard]] Result* get() const
    {
        return result.get();
    }

   protected:
    std::shared_ptr<bes::dbal::wide::Result> result;
};

}  // namespace bes::dbal::wide