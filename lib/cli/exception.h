#ifndef BES_CLI_EXCEPTION_H
#define BES_CLI_EXCEPTION_H

#include "lib/bes/bes.h"

namespace bes::cli {

class CliException : public BesException
{
   public:
    CliException(std::string msg) : BesException(msg, bes::ExitCode::CLI_PARSE_ERR) {}
    CliException(std::string msg, bes::ExitCode e) : BesException(msg, e) {}
};

class ArgumentConflictException : public CliException
{
    using CliException::CliException;
};

class MalformedArgumentException : public CliException
{
    using CliException::CliException;
};

class NoValueException : public CliException
{
    using CliException::CliException;
};

class ValueErrorException : public CliException
{
    using CliException::CliException;
};

class UnexpectedValueException : public CliException
{
    using CliException::CliException;
};

class NoSuchArgumentException : public CliException
{
    using CliException::CliException;
};

}  // namespace bes::cli

#endif
