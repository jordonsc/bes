#pragma once

#include <any>
#include <memory>
#include <string>
#include <vector>

#include "shell_interface.h"

namespace bes::templating::data {

template <class T>
class StandardShell : public ShellInterface
{
   public:
    StandardShell(T item) : item(item) {}

   protected:
    T item;
};

/**
 * A context map for something that contains native support for output streams (operator<<()).
 */
template <class T>
class SimpleShell : public ShellInterface
{
   public:
    SimpleShell(T item) : item(item) {}

    inline void render(std::ostringstream& str) const override
    {
        str << item;
    }

   protected:
    T item;
};

/**
 * A context map for integral number types.
 */
template <class T>
class IntegerShell : public ShellInterface
{
   public:
    IntegerShell(T item) : item(item) {}

    inline void render(std::ostringstream& str) const override
    {
        str << item;
    }

    bool isTrue() const override
    {
        return item != 0;
    }

    long asInt() const override
    {
        return static_cast<long>(item);
    }

   protected:
    T item;
};

/**
 * A context map for floating-point number types.
 */
template <class T>
class FloatShell : public ShellInterface
{
   public:
    FloatShell(T item) : item(item) {}

    inline void render(std::ostringstream& str) const override
    {
        str << item;
    }

    bool isTrue() const override
    {
        return item != 0;
    }

    double asFloat() const override
    {
        return static_cast<double>(item);
    }

   protected:
    T item;
};

/**
 * Something that takes a normal forward iterator
 */
template <class T>
class IterableShell : public ShellInterface
{
   public:
    IterableShell(T item) : item(item) {}

    size_t count() const override
    {
        return item.size();
    }

    void begin() override
    {
        it = item.begin();
    }

    bool isTrue() const override
    {
        return item.size() > 0;
    }

   protected:
    T item;
    typename T::iterator it;
};

/**
 * A pointer reference to something that takes a normal forward iterator
 */
template <class T>
class IterablePtrShell : public ShellInterface
{
   public:
    IterablePtrShell(T* item) : item(item) {}

    size_t count() const override
    {
        it = item->begin();
        return item->size();
    }

    void begin() override
    {
        it = item->begin();
    }

    bool isTrue() const override
    {
        return item->size() > 0;
    }

   protected:
    T* item;
    typename T::iterator it;
};

/// Number types
template <>
class StandardShell<char> : public IntegerShell<char>
{
    using IntegerShell::IntegerShell;
};

template <>
class StandardShell<int16_t> : public IntegerShell<int16_t>
{
    using IntegerShell::IntegerShell;
};

template <>
class StandardShell<uint16_t> : public IntegerShell<uint16_t>
{
    using IntegerShell::IntegerShell;
};

template <>
class StandardShell<int32_t> : public IntegerShell<int32_t>
{
    using IntegerShell::IntegerShell;
};

template <>
class StandardShell<uint32_t> : public IntegerShell<uint32_t>
{
    using IntegerShell::IntegerShell;
};

template <>
class StandardShell<int64_t> : public IntegerShell<int64_t>
{
    using IntegerShell::IntegerShell;
};

template <>
class StandardShell<uint64_t> : public IntegerShell<uint64_t>
{
    using IntegerShell::IntegerShell;
};

template <>
class StandardShell<float> : public FloatShell<float>
{
    using FloatShell::FloatShell;
};

template <>
class StandardShell<double> : public FloatShell<double>
{
    using FloatShell::FloatShell;
};

/// BOOL
template <>
class StandardShell<bool> : public ShellInterface
{
   public:
    StandardShell(bool item) : item(item) {}

    inline void render(std::ostringstream& str) const override
    {
        str << std::boolalpha << item;
    }

    bool isTrue() const override
    {
        return item;
    }

    long asInt() const override
    {
        return item ? 1 : 0;
    }

   protected:
    bool item;
};

/// STRING
template <>
class StandardShell<std::string> : public SimpleShell<std::string>
{
    using SimpleShell::SimpleShell;

   public:
    inline std::shared_ptr<ShellInterface> childNode(std::string const& key) const override
    {
        if (key == "length") {
            return std::make_shared<StandardShell<size_t>>(item.length());
        } else {
            throw IndexErrorException(BES_TEMPLATING_NO_NODE + key);
        }
    }

    bool isTrue() const override
    {
        return item.length() > 0;
    }
};

/// STRING const*
template <>
class StandardShell<std::string const*> : public SimpleShell<std::string const*>
{
    using SimpleShell::SimpleShell;

   public:
    void render(std::ostringstream& str) const override
    {
        str << *item;
    }

    inline std::shared_ptr<ShellInterface> childNode(std::string const& key) const override
    {
        if (key == "length") {
            return std::make_shared<StandardShell<size_t>>(item->length());
        } else {
            throw IndexErrorException(BES_TEMPLATING_NO_NODE + key);
        }
    }

    bool isTrue() const override
    {
        return item->length() > 0;
    }
};

/// SHARED_PTR<STRING>
template <>
class StandardShell<std::shared_ptr<std::string>> : public SimpleShell<std::shared_ptr<std::string>>
{
    using SimpleShell::SimpleShell;

   public:
    void render(std::ostringstream& str) const override
    {
        str << *item;
    }

    inline std::shared_ptr<ShellInterface> childNode(std::string const& key) const override
    {
        if (key == "length") {
            return std::make_shared<StandardShell<size_t>>(item->length());
        } else {
            throw IndexErrorException(BES_TEMPLATING_NO_NODE + key);
        }
    }

    bool isTrue() const override
    {
        return item->length() > 0;
    }
};

/// VECTOR<STRING>
template <>
class StandardShell<std::vector<std::string>> : public IterableShell<std::vector<std::string>>
{
    using IterableShell::IterableShell;

   public:
    std::shared_ptr<ShellInterface> yield() override
    {
        return std::make_shared<StandardShell<std::string>>(*it++);
    }
};

/// VECTOR<STRING> (ptr)
template <>
class StandardShell<std::vector<std::string>*> : public IterablePtrShell<std::vector<std::string>>
{
    using IterablePtrShell::IterablePtrShell;

   public:
    std::shared_ptr<ShellInterface> yield() override
    {
        return std::make_shared<StandardShell<std::string>>(*it++);
    }
};

}  // namespace bes::templating::data

