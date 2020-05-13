#ifndef BES_TEMPLATING_DATA_CONTEXT_H
#define BES_TEMPLATING_DATA_CONTEXT_H

#include <memory>
#include <shared_mutex>
#include <unordered_map>
#include <vector>

#include "../exception.h"
#include "shell_interface.h"

namespace bes::templating::data {

class Context
{
   public:
    Context();

    Context& IncreaseStack();
    Context& DecreaseStack();

    Context& SetValue(std::string const& key, std::shared_ptr<ShellInterface> item);
    std::shared_ptr<ShellInterface> const& GetValue(std::string const& key);

   protected:
    std::vector<std::unordered_map<std::string, std::shared_ptr<ShellInterface>>> data;
    std::shared_mutex value_mutex;
};

}  // namespace bes::templating::data

#endif
