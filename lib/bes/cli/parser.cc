#include "parser.h"

#include <iostream>
#include <utility>
#include <vector>

using namespace bes::cli;

/**
 * Validates the parameters and then iterates over all existing args to ensure there are no conflicts.
 *
 * Will always have O(n) efficiency as the short_form variable is optional and not indexed, yet needs checking.
 */
void Parser::validateArgument(Arg& arg)
{
    if (arg.long_form.length() == 0 || !isValidLongArg(arg.long_form)) {
        throw MalformedArgumentException("Argument long-form is malformed");
    }

    if (arg.short_form != 0 && !isValidShortArg(arg.short_form)) {
        throw MalformedArgumentException("Argument short-form must be [a-z] or [A-Z]");
    }

    for (auto const& it : arg_list) {
        if (arg.long_form == it.second.long_form) {
            throw ArgumentConflictException("Argument conflict on --" + arg.long_form);
        }

        if (arg.short_form != 0 && it.second.short_form != 0 && arg.short_form == it.second.short_form) {
            throw ArgumentConflictException(std::string("Argument conflict on -") + arg.short_form);
        }
    }
}

Parser& Parser::operator<<(Arg arg)
{
    addArgument(std::move(arg));
    return *this;
}

Parser& Parser::addArgument(Arg arg)
{
    validateArgument(arg);

    std::lock_guard<std::mutex> lock(_m);

    arg_list.insert({arg.long_form, arg});
    return *this;
}

Parser& Parser::addArgument(char s, std::string l, std::string default_val)
{
    return addArgument(Arg(s, std::move(l), std::move(default_val)));
}

size_t Parser::argCount() const
{
    return arg_list.size();
}

/**
 * Short-arg must be [a-z] or [A-Z]
 */
bool Parser::isValidShortArg(char const& c)
{
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

/**
 * Long-arg must contain only lower-case letters
 */
bool Parser::isValidLongArg(std::string const& arg)
{
    try {
        std::for_each(arg.begin(), arg.end(), [](const char& c) {
            if (c < 'a' || c > 'z') {
                throw MalformedArgumentException("");
            }
        });
    } catch (MalformedArgumentException&) {
        return false;
    }

    return true;
}

// Non-const
Arg& Parser::getArg(std::string const& s)
{
    auto it = arg_list.find(s);
    if (it != arg_list.end()) {
        return it->second;
    } else {
        throw NoSuchArgumentException("Argument '--" + s + "' does not exist");
    }
}

// Const implementation
Arg const& Parser::operator[](std::string const& s) const
{
    auto it = arg_list.find(s);
    if (it != arg_list.end()) {
        return it->second;
    } else {
        throw NoSuchArgumentException("Argument '--" + s + "' does not exist");
    }
}

void Parser::parse(int const& argc, char** const argv)
{
    if (argc >= 2) {
        std::lock_guard<std::mutex> lock(_m);

        // To allow the GNU-style -- argument from precluding further options
        bool allow_options = true;

        // We'll reference the previous option when assigning value-arguments
        _last_arg = nullptr;

        for (int arg_index = 1; arg_index < argc; ++arg_index) {
            char* arg = argv[arg_index];
            BES_LOG(TRACE) << "Parse arg `" << arg << "`";

            if (allow_options && strlen(arg) > 1 && arg[0] == '-') {
                if (arg[1] == '-') {
                    if (strlen(arg) == 2) {
                        // double-hyphen
                        BES_LOG(TRACE) << " - precluding further options";
                        allow_options = false;
                        _last_arg = nullptr;
                        continue;
                    }

                    // Long-form
                    parseLong(arg + 2);
                } else {
                    // Short-form
                    parseShort(arg + 1);
                }
            } else {
                if (_last_arg == nullptr || _last_arg->value_type == ValueType::NONE) {
                    // Can NOT accept a value, add as positional
                    BES_LOG(TRACE) << " - added as positional";
                    positionals.emplace_back(arg);
                } else {
                    // CAN accept a value, add to last argument
                    _last_arg->setValue(arg);
                }

                // Last argument wasn't an option, so we can't reference it
                _last_arg = nullptr;
            }
        }
    } else {
        BES_LOG(DEBUG) << "Nothing to parse on CLI";
    }

    validateRequired();
}

/**
 * Assuming we have an argument in the form -a or -abc, we'll should pass to this function a char* starting after the
 * hyphen. We'll iterate over the 'abc' and update Arg's on the Parser.
 */
void Parser::parseShort(char* short_args)
{
    char short_arg = 0;
    Arg* arg = nullptr;

    // Step over chars
    while ((short_arg = *short_args++) != 0) {
        if (short_arg == '=') {
            // Move into value acquisition
            if (_last_arg == nullptr) {
                // This would basically be "-=" or something stupid
                throw MalformedArgumentException(std::string("Unexpected character '") + short_arg + "' in argument '" +
                                                 short_args + "'");
            }

            // Will throw an UnexpectedValueException if it doesn't accept a value
            _last_arg->setValue(short_args);
            break;
        }

        if (!isValidShortArg(short_arg)) {
            throw MalformedArgumentException(std::string("Argument -") + short_arg + " is not valid");
        }

        arg = getArgFromShort(short_arg);
        if (arg == nullptr) {
            throw NoSuchArgumentException(std::string("Argument -") + short_arg + " does not exist");
        }

        // Increase the arg's appearance count
        _last_arg = arg;
        ++(*arg);
    }
}

void Parser::parseLong(std::string const& long_arg)
{
    auto pos = long_arg.find('=');
    if (pos != std::string::npos) {
        // Option contains a value, needs to be split
        std::string arg_name = std::string(long_arg, 0, pos);
        std::string arg_value = std::string(long_arg, pos + 1);

        Arg& arg = getArg(arg_name);
        ++arg.setValue(std::move(arg_value));
        _last_arg = &arg;
    } else {
        // Simple option, no value
        Arg& arg = getArg(long_arg);
        ++arg;
        _last_arg = &arg;
    }
}

/**
 * A rather inefficient iterate-and-retrieve on the short value of the argument. Will return nullptr if a matching Arg
 * is not found.
 */
Arg* Parser::getArgFromShort(char c)
{
    for (auto& it : arg_list) {
        if (it.second.short_form == c) {
            return &it.second;
        }
    }

    return nullptr;
}

/**
 * Throws an exception is an argument that requires a value does not have one.
 */
void Parser::validateRequired()
{
    std::lock_guard<std::mutex> lock(_m);

    for (auto& arg : arg_list) {
        if (arg.second.arg_value.empty()) {
            if (arg.second.value_type == ValueType::REQUIRED && arg.second.present()) {
                throw NoValueException("Argument --" + arg.second.long_form + " requires a value");
            } else if (arg.second.value_type == ValueType::MANDATORY) {
                throw NoValueException("Argument --" + arg.second.long_form + " is mandatory");
            }
        }
    }
}

std::vector<Arg const*> Parser::getAllArgs() const
{
    std::vector<Arg const*> vec;
    for (auto const& a : arg_list) {
        vec.push_back(&(a.second));
    }

    return vec;
}
