#pragma once

#include <bes/log.h>

#include <map>
#include <mutex>
#include <vector>

#include "arg.h"
#include "exception.h"

namespace bes::cli {

class Parser
{
   public:
    Parser& operator<<(Arg arg);
    Parser& addArgument(Arg arg);
    Parser& addArgument(char s, std::string l, std::string default_val = "");
    [[nodiscard]] size_t argCount() const;

    Arg const& operator[](std::string const& s) const;
    [[nodiscard]] std::vector<Arg const*> getAllArgs() const;

    void parse(int const& argc, char** argv);

   protected:
    /**
     * Raises an ArgumentConflictException if there is a conflict between existing arguments.
     */
    void validateArgument(Arg& arg);

    /**
     * Raising a NoValueException if an argument doesn't have a required value
     */
    void validateRequired();

    std::map<std::string, Arg> arg_list;
    std::vector<std::string> positionals;

    static bool isValidShortArg(char const& c);
    static bool isValidLongArg(std::string const& arg);
    Arg& getArg(std::string const& s);

   private:
    void parseShort(char* short_args);
    void parseLong(const std::string& long_arg);

    Arg* getArgFromShort(char c);

    // Inner workings of parser
    Arg* _last_arg = nullptr;
    std::mutex _m;
};

}  // namespace bes::cli
