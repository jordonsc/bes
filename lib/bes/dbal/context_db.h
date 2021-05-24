#pragma once

#include "context.h"

namespace bes::dbal {

class ContextualDatabase
{
   public:
    ContextualDatabase() = delete;
    explicit ContextualDatabase(Context c) : context(std::move(c)) {}

   protected:
    [[nodiscard]] Context const& getContext() const
    {
        return context;
    }

   private:
    Context context;
};

}  // namespace bes::dbal