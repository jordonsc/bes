#ifndef BES_TEMPLATING_DATA_CONTEXT_H
#define BES_TEMPLATING_DATA_CONTEXT_H

#include <list>
#include <memory>
#include <shared_mutex>
#include <unordered_map>
#include <vector>

#include "../exception.h"
#include "shell_interface.h"

namespace bes::templating::node {
class RootNode;
}  // namespace bes::templating::node

namespace bes::templating::data {

class Context
{
   public:
    Context();

    Context& IncreaseStack();
    Context& DecreaseStack();

    Context& SetValue(std::string const& key, std::shared_ptr<ShellInterface> item);
    std::shared_ptr<ShellInterface> const& GetValue(std::string const& key);

    void PrependTemplate(bes::templating::node::RootNode const* tmp);
    [[nodiscard]] bes::templating::node::RootNode const* GetTemplate() const;
    void NextTemplate();
    void PrevTemplate();

   protected:
    std::vector<std::unordered_map<std::string, std::shared_ptr<ShellInterface>>> data;
    std::list<node::RootNode const*> template_stack;
    std::list<node::RootNode const*>::iterator template_iterator;
    std::shared_mutex value_mutex;
};

}  // namespace bes::templating::data

#endif
