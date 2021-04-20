#include "shell_interface.h"

using namespace bes::templating::data;

/**
 * Rendering your main value.
 *
 * Not all objects have a renderable 'main value', it's OK not to implement this.
 */
void ShellInterface::render(std::ostringstream& str) const
{
    throw UnknownTypeException(BES_TEMPLATING_NO_SHELL);
}

/**
 * Used for referencing member objects.
 */
std::shared_ptr<ShellInterface> ShellInterface::childNode(std::string const& key) const
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
        long left = asInt();

        try {
            return left < rhs.asInt();
        } catch (ValueErrorException const&) {
            return (double)left < rhs.asFloat();
        }
    } catch (ValueErrorException const&) {
        double left = asFloat();
        try {
            return left < rhs.asFloat();
        } catch (ValueErrorException const&) {
            return left < (double)(rhs.asInt());
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
        long left = asInt();

        try {
            return left == rhs.asInt();
        } catch (ValueErrorException const&) {
            return (double)left == rhs.asFloat();
        }
    } catch (ValueErrorException const&) {
        double left = asFloat();
        try {
            return left == rhs.asFloat();
        } catch (ValueErrorException const&) {
            return left == (double)(rhs.asInt());
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
bool ShellInterface::isTrue() const
{
    return true;
}

/**
 * Implement one of EITHER AsInt() or AsFloat(), but not both, and only if it is appropriate to represent yourself
 * as a number.
 */
long ShellInterface::asInt() const
{
    throw ValueErrorException(BES_TEMPLATING_NOT_COMPATIBLE);
}

/**
 * Implement one of EITHER AsInt() or AsFloat(), but not both, and only if it is appropriate to represent yourself
 * as a number.
 */
double ShellInterface::asFloat() const
{
    throw ValueErrorException(BES_TEMPLATING_NOT_COMPATIBLE);
}

/**
 * Retrieve the size of the data contained within an iterable object.
 */
size_t ShellInterface::count() const
{
    throw ValueErrorException(BES_TEMPLATING_NO_SHELL);
}

/**
 * Reset the internal iterator in preparation for a loop.
 */
void ShellInterface::begin()
{
    throw IndexErrorException(BES_TEMPLATING_NO_SHELL);
}

/**
 * Yield an iterable object and increment internal iterator.
 */
std::shared_ptr<ShellInterface> ShellInterface::yield()
{
    throw IndexErrorException(BES_TEMPLATING_NO_SHELL);
}