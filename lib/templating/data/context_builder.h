#ifndef BES_TEMPLATING_DATA_HELPER_H
#define BES_TEMPLATING_DATA_HELPER_H

#include "context.h"
#include "std_shells.h"

namespace bes::templating::data {

class ContextBuilder
{
   public:
    ContextBuilder(data::Context& ctx) : ctx(ctx) {}
    ContextBuilder() : ctx(local) {}

    template <class T>
    void Set(std::string const& key, T item);

    inline data::Context& GetContext() const
    {
        return ctx;
    }

   protected:
    data::Context& ctx;
    data::Context local;
};

template <class T>
inline void ContextBuilder::Set(std::string const& key, T item)
{
    ctx.SetValue(key, std::make_shared<StandardShell<T>>(item));
}

template <>
inline void ContextBuilder::Set(std::string const& key, char* item)
{
    ctx.SetValue(key, std::make_shared<StandardShell<std::string>>(std::string(item)));
}

template <>
inline void ContextBuilder::Set(std::string const& key, char const* item)
{
    ctx.SetValue(key, std::make_shared<StandardShell<std::string>>(std::string(item)));
}

}  // namespace bes::templating::data

#endif
