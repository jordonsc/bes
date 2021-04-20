#pragma once

#include "context.h"
#include "std_shells.h"

namespace bes::templating::data {

class ContextBuilder
{
   public:
    explicit ContextBuilder(data::Context& ctx) : ctx(ctx) {}
    ContextBuilder() : ctx(local) {}

    template <class T>
    void set(std::string const& key, T* item);

    template <class T>
    void set(std::string const& key, T const* item);

    template <class T>
    void set(std::string const& key, T item);

    [[nodiscard]] inline data::Context& getContext() const
    {
        return ctx;
    }

   protected:
    data::Context& ctx;
    data::Context local;
};

template <class T>
inline void ContextBuilder::set(std::string const& key, T* item)
{
    ctx.setValue(key, std::make_shared<StandardShell<T const*>>(item));
}

template <class T>
inline void ContextBuilder::set(std::string const& key, T const* item)
{
    ctx.setValue(key, std::make_shared<StandardShell<T const*>>(item));
}

template <class T>
inline void ContextBuilder::set(std::string const& key, T item)
{
    ctx.setValue(key, std::make_shared<StandardShell<T>>(item));
}

template <>
inline void ContextBuilder::set(std::string const& key, char* item)
{
    ctx.setValue(key, std::make_shared<StandardShell<std::string>>(std::string(item)));
}

template <>
inline void ContextBuilder::set(std::string const& key, char const* item)
{
    ctx.setValue(key, std::make_shared<StandardShell<std::string>>(std::string(item)));
}

}  // namespace bes::templating::data
