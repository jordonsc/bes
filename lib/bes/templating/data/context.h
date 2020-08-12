#ifndef BES_TEMPLATING_DATA_CONTEXT_H
#define BES_TEMPLATING_DATA_CONTEXT_H

#include <memory>
#include <shared_mutex>
#include <unordered_map>
#include <vector>

#include "../exception.h"
#include "shell_interface.h"

namespace bes::templating::node {

class Node;
class RootNode;

}  // namespace bes::templating::node

namespace bes::templating::data {

class Context
{
   public:
    Context();

    /// Increase/decrease scope
    Context& IncreaseStack();
    Context& DecreaseStack();

    /// Data values stored in the context
    Context& SetValue(std::string const& key, std::shared_ptr<ShellInterface> item);
    std::shared_ptr<ShellInterface> const& GetValue(std::string const& key);

    /// Macro pool - unique to an inheritance line (the template entry-point)
    void AddMacro(std::string const& key, node::Node const* node);
    bool HasMacro(std::string const& key) const;
    node::Node const* GetMacro(std::string const& key) const;

   protected:
    std::vector<std::unordered_map<std::string, std::shared_ptr<ShellInterface>>> data;
    std::shared_mutex value_mutex;
    std::unordered_map<std::string, node::Node const*> macros;
};

}  // namespace bes::templating::data

#endif
