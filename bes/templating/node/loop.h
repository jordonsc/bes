#ifndef BES_TEMPLATING_SYNTAX_LOOP_H
#define BES_TEMPLATING_SYNTAX_LOOP_H

#include "../data/std_shells.h"

namespace bes::templating::node {

/**
 * A loop context that can be used inside of for loops
 *
 * Usable nodes:
 *   int    loop.index:    zero-indexed position in loop
 *   int    loop.index1:   one-indexed position in loop
 *   int    loop.size:     size of the array
 *   bool   loop.first:    are we in the first element of the loop?
 *   bool   loop.last:     are we in the last element of the loop?
 *   bool   loop.even:     is this an even index (1-indexed)
 *   bool   loop.odd:      is this an odd index (1-indexed)
 */
struct loop
{
    loop() = default;
    loop(size_t index, size_t size) : index(index), size(size) {}

    size_t index;
    size_t size;
};

}  // namespace bes::templating::node

namespace bes::templating::data {

template <>
class StandardShell<node::loop> : public ShellInterface
{
   public:
    explicit StandardShell(const node::loop& item) : item(item) {}

    inline void Render(std::ostringstream& ss) const override
    {
        throw TemplateException("Cannot render the loop context");
    }

    [[nodiscard]] inline std::shared_ptr<ShellInterface> ChildNode(std::string const& key) const override
    {
        if (key == "index") {
            return std::make_shared<StandardShell<size_t>>(item.index);
        } else if (key == "index1") {
            return std::make_shared<StandardShell<size_t>>(item.index + 1);
        } else if (key == "odd") {
            return std::make_shared<StandardShell<bool>>(item.index % 2 == 0);
        } else if (key == "even") {
            return std::make_shared<StandardShell<bool>>(item.index % 2 == 1);
        } else if (key == "size") {
            return std::make_shared<StandardShell<size_t>>(item.size);
        } else if (key == "first") {
            return std::make_shared<StandardShell<bool>>(item.index == 0);
        } else if (key == "last") {
            return std::make_shared<StandardShell<bool>>((item.index + 1) == item.size);
        } else {
            throw IndexErrorException("Loop context does not exist: " + key);
        }
    }

   protected:
    node::loop item;
};

}  // namespace bes::templating::data

#endif