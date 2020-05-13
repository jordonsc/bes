#ifndef BES_TEMPLATING_DATA_SHELL_INTERFACE_H
#define BES_TEMPLATING_DATA_SHELL_INTERFACE_H

#define BES_TEMPLATING_NO_NODE "No such child node: "
#define BES_TEMPLATING_NOT_ITERABLE "Not iterable"
#define BES_TEMPLATING_NOT_RENDERABLE "Not renderable"
#define BES_TEMPLATING_NO_SHELL "No shell implementation"
#define BES_TEMPLATING_NOT_COMPATIBLE "Not a compatible conversion"

#include <memory>
#include <sstream>

#include "../exception.h"

namespace bes::templating::data {

class ShellInterface
{
   public:
    /**
     * Rendering your main value.
     *
     * Not all objects have a renderable 'main value', it's OK not to implement this.
     */
    virtual void Render(std::ostringstream& str) const;

    /**
     * Used for referencing member objects.
     */
    [[nodiscard]] virtual std::shared_ptr<ShellInterface> ChildNode(std::string const& key) const;

    /**
     * Boolean test.
     *
     * Strongly recommended to override this, the default is "yes, I exist". Don't throw an exception here if there is
     * no boolean representation of your class, instead return true, so that an `if MyClass` will return true.
     */
    [[nodiscard]] virtual bool IsTrue() const;

    /**
     * Implement one of EITHER AsInt() or AsFloat(), but not both, and only if it is appropriate to represent yourself
     * as a number.
     */
    [[nodiscard]] virtual long AsInt() const;

    /**
     * Implement one of EITHER AsInt() or AsFloat(), but not both, and only if it is appropriate to represent yourself
     * as a number.
     */
    [[nodiscard]] virtual double AsFloat() const;

    /**
     * Retrieve the size of the data contained within an iterable object.
     */
    [[nodiscard]] virtual size_t Count() const;

    /**
     * Reset the internal iterator in preparation for a loop.
     */
    virtual void Begin();

    /**
     * Yield an iterable object and increment internal iterator.
     */
    virtual std::shared_ptr<ShellInterface> Yield();

    virtual bool operator<(ShellInterface const& rhs) const;
    virtual bool operator>(ShellInterface const& rhs) const;
    virtual bool operator<=(ShellInterface const& rhs) const;
    virtual bool operator>=(ShellInterface const& rhs) const;
    virtual bool operator==(ShellInterface const& rhs) const;
    virtual bool operator!=(ShellInterface const& rhs) const;

    friend inline std::ostringstream& operator<<(std::ostringstream& css, ShellInterface const& shell)
    {
        shell.Render(css);
        return css;
    }

    friend inline std::ostringstream& operator<<(std::ostringstream& css, ShellInterface const* shell)
    {
        shell->Render(css);
        return css;
    }
};

}  // namespace bes::templating::data

#endif
