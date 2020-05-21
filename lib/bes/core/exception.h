#ifndef BES_BES_EXCEPTION_H
#define BES_BES_EXCEPTION_H

#include <filesystem>
#include <stdexcept>
#include <utility>

#include "model.h"

namespace bes {

class BesException : public std::exception
{
   public:
    explicit BesException(std::string msg, int exit_code = 1) : err_msg(std::move(msg)), exit_code(exit_code){};
    BesException(std::string msg, bes::ExitCode exit_flag)
        : err_msg(std::move(msg)), exit_code(static_cast<int>(exit_flag)){};

    [[nodiscard]] const char* what() const noexcept override
    {
        return err_msg.c_str();
    }

    [[nodiscard]] virtual std::string const& ErrorMessage() const noexcept
    {
        return err_msg;
    }

    [[nodiscard]] virtual int ExitCode() const noexcept
    {
        return exit_code;
    }

   private:
    std::string const err_msg;
    int const exit_code;
};

/**
 * Exiting with this exception does not attempt to log anything or inform the user of the cause. Anything in the
 * ErrorMessage() will be outputted to stderr, otherwise no further output will occur.
 */
class ManagedExitException : public BesException
{
    using BesException::BesException;
};

/**
 * Throw this to exit the application with an exit code and absolutely no logging or output to stderr, regardless of the
 * contents of ErrorMessage().
 */
class SilentExitException : public BesException
{
    using BesException::BesException;
};

class IndexErrorException : public BesException
{
    using BesException::BesException;
};

class NullException : public bes::IndexErrorException
{
   public:
    explicit NullException(std::string key) : IndexErrorException("Key is null"), key(std::move(key)) {}
    NullException(std::string key, std::string const& msg) : IndexErrorException(msg), key(std::move(key)) {}
    NullException(std::string key, std::string const& msg, int exit_code)
        : IndexErrorException(msg, exit_code), key(std::move(key))
    {}
    NullException(std::string key, std::string const& msg, bes::ExitCode exit_code)
        : IndexErrorException(msg, exit_code), key(std::move(key))
    {}

    [[nodiscard]] inline constexpr std::string const& GetKey() const noexcept
    {
        return key;
    }

   private:
    std::string const key;
};

class ContainerException : public BesException
{
    using BesException::BesException;
};

class KeyExistsException : public ContainerException
{
    using ContainerException::ContainerException;
};

class KeyNotFoundException : public ContainerException
{
    using ContainerException::ContainerException;
};

class FileSystemException : public bes::BesException
{
   public:
    FileSystemException(std::string msg, std::string fn, bes::ExitCode exit_flag)
        : BesException(std::move(msg), exit_flag), fn(std::move(fn))
    {}

    FileSystemException(std::string msg, std::string fn, int exit_code = 1)
        : BesException(std::move(msg), exit_code), fn(std::move(fn))
    {}

    virtual inline std::string& GetPath()
    {
        return fn;
    }

   private:
    std::string fn;
};

class FileNotFoundException : public FileSystemException
{
    using FileSystemException::FileSystemException;
};

}  // namespace bes

#endif
