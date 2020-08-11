#ifndef BES_CLI_PARSER_H
#define BES_CLI_PARSER_H

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
    Parser& AddArgument(Arg arg);
    Parser& AddArgument(char s, std::string l, std::string default_val = "");
    [[nodiscard]] size_t ArgCount() const;

    Arg const& operator[](std::string const& s) const;
    [[nodiscard]] std::vector<Arg const*> GetAllArgs() const;

    void Parse(int const& argc, char** argv);

   protected:
    /**
     * Raises an ArgumentConflictException if there is a conflict between existing arguments.
     */
    void ValidateArgument(Arg& arg);

    /**
     * Raising a NoValueException if an argument doesn't have a required value
     */
    void ValidateRequired();

    std::map<std::string, Arg> arg_list;
    std::vector<std::string> positionals;

    static bool IsValidShortArg(char const& c);
    static bool IsValidLongArg(std::string const& arg);
    Arg& GetArg(std::string const& s);

   private:
    void ParseShort(char* short_args);
    void ParseLong(const std::string& long_arg);

    Arg* GetArgFromShort(char c);

    // Inner workings of parser
    Arg* _last_arg = nullptr;
    std::mutex _m;
};

}  // namespace bes::cli

#endif
