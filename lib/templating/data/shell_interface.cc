#include "shell_interface.h"

using namespace bes::templating::data;

/**
 * Rendering your main value.
 *
 * Not all objects have a renderable 'main value', it's OK not to implement this.
 */
void ShellInterface::Render(std::ostringstream& str) const
{
    throw UnknownTypeException(BES_TEMPLATING_NO_SHELL);
}

/**
 * Used for referencing member objects.
 */
std::shared_ptr<ShellInterface> ShellInterface::ChildNode(std::string const& key) const
{
    throw UnknownTypeException(std::string(BES_TEMPLATING_NO_SHELL) + " (" + key + ")");
}

/**
 * We'll first attempt this as an int comparison, then try the floating-point comparison if that fails. We'll also
 * give priority to matching types before transforming an int to a float.
 *
 * If either objects cannot output either an int or a float, this function will throw a ValueErrorException.
 */
bool ShellInterface::operator<(ShellInterface const& rhs) const
{
    try {
        long left = AsInt();

        try {
            return left < rhs.AsInt();
        } catch (ValueErrorException const&) {
            return left < rhs.AsFloat();
        }
    } catch (ValueErrorException const&) {
        double left = AsFloat();
        try {
            return left < rhs.AsFloat();
        } catch (ValueErrorException const&) {
            return left < rhs.AsInt();
        }
    }
}

bool ShellInterface::operator>(ShellInterface const& rhs) const
{
    return rhs < *this;
}

bool ShellInterface::operator<=(ShellInterface const& rhs) const
{
    return !(rhs < *this);
}

bool ShellInterface::operator>=(ShellInterface const& rhs) const
{
    return !(*this < rhs);
}

/**
 * The equality operators will use the comparison operator logic to calculate a numeric match only. If this fails
 * with a ValueErrorException, you could try rendering the objects to do a string comparison.
 */
bool ShellInterface::operator==(ShellInterface const& rhs) const
{
    try {
        long left = AsInt();

        try {
            return left == rhs.AsInt();
        } catch (ValueErrorException const&) {
            return left == rhs.AsFloat();
        }
    } catch (ValueErrorException const&) {
        double left = AsFloat();
        try {
            return left == rhs.AsFloat();
        } catch (ValueErrorException const&) {
            return left == rhs.AsInt();
        }
    }
}

bool ShellInterface::operator!=(ShellInterface const& rhs) const
{
    return !(*this == rhs);
}

/**
 * Boolean test.
 *
 * Strongly recommended to override this, the default is "yes, I exist". Don't throw an exception here if there is
 * no boolean representation of your class, instead return true, so that an `if MyClass` will return true.
 */
bool ShellInterface::IsTrue() const
{
    return true;
}

/**
 * Implement one of EITHER AsInt() or AsFloat(), but not both, and only if it is appropriate to represent yourself
 * as a number.
 */
long ShellInterface::AsInt() const
{
    throw ValueErrorException(BES_TEMPLATING_NOT_COMPATIBLE);
}

/**
 * Implement one of EITHER AsInt() or AsFloat(), but not both, and only if it is appropriate to represent yourself
 * as a number.
 */
double ShellInterface::AsFloat() const
{
    throw ValueErrorException(BES_TEMPLATING_NOT_COMPATIBLE);
}

/**
 * Retrieve the size of the data contained within an iterable object.
 */
size_t ShellInterface::Count() const
{
    throw ValueErrorException(BES_TEMPLATING_NO_SHELL);
}

/**
 * Reset the internal iterator in preparation for a loop.
 */
void ShellInterface::Begin()
{
    throw IndexErrorException(BES_TEMPLATING_NO_SHELL);
}

/**
 * Yield an iterable object and increment internal iterator.
 */
std::shared_ptr<ShellInterface> ShellInterface::Yield()
{
    throw IndexErrorException(BES_TEMPLATING_NO_SHELL);
}