#pragma once

#include <algorithm>
#include <cctype>
#include <locale>
#include <sstream>
#include <string>
#include <vector>

#include "exception.h"

namespace bes::templating {

using Filter = std::function<void(std::string &)>;

struct Text
{
    /**
     * Break a string into words, loading a vector.
     */
    static inline std::vector<std::string> split(std::string const &str, std::string const &delim = " ")
    {
        std::vector<std::string> tokens;
        size_t prev = 0, pos = 0;
        do {
            pos = str.find(delim, prev);
            if (pos == std::string::npos) {
                pos = str.length();
            }
            std::string token = str.substr(prev, pos - prev);

            if (!token.empty()) {
                tokens.push_back(token);
            }

            prev = pos + delim.length();
        } while (pos < str.length() && prev < str.length());

        return tokens;
    }

    /**
     * Split into a vector of arguments (tokens), delimited by a space or a pipe (|), but:
     * - the pipe counts as a token, a space does not
     * - square brackets and quotes enclose a single token
     */
    static inline std::vector<std::string> splitArgs(std::string const &str)
    {
        std::vector<std::string> tokens;
        std::ostringstream token;
        size_t token_len = 0;

        // The boundary marker represents the closing parenthesis we expect. If it has a value, it means we're inside
        // some kind of parenthesis block (quotes, square brackets, etc). If it is set to space, it means we've closed
        // the parentheses but are still expecting whitespace before the next token can begin.
        char boundary = 0;

        for (char c : str) {
            if ((c == ' ' || c == '|' || c == '(') && (!boundary || boundary == ' ')) {
                if (token_len) {
                    tokens.push_back(token.str());
                    token.str(std::string());
                    token_len = 0;
                    boundary = 0;
                }

                if (c == '(') {
                    boundary = ')';
                    token << c;
                    token_len = 1;
                } else if (c == '|') {
                    tokens.emplace_back("|");
                }

                continue;
            }

            // Closing boundary found, set the boundary to whitespace to allow for trailing whitespace
            if (boundary && c == boundary) {
                boundary = ' ';
            }

            // Opening boundary
            if (token_len == 0 && c == '"') {
                boundary = '"';
            } else if (token_len == 0 && c == '[') {
                boundary = ']';
            } else if (token_len == 0 && c == '(') {
                boundary = ')';
            }

            ++token_len;
            token << c;
        }

        if (boundary && boundary != ' ') {
            throw TemplateException("Unterminated symbol: " + str);
        }

        if (token_len) {
            tokens.push_back(token.str());
        }

        return tokens;
    }

    /**
     * Remove whitespace from the front of a string
     */
    static inline void trimFront(std::string &s)
    {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
                    return !std::isspace(ch);
                }));
    }

    /**
     * Remove whitespace from the back of a string
     */
    static inline void trimBack(std::string &s)
    {
        s.erase(std::find_if(s.rbegin(), s.rend(),
                             [](int ch) {
                                 return !std::isspace(ch);
                             })
                    .base(),
                s.end());
    }

    /**
     * Remove whitespace from both ends of a string
     */
    static inline void trim(std::string &s)
    {
        trimFront(s);
        trimBack(s);
    }

    static void replaceAll(std::string &str, std::string const &from, std::string const &to)
    {
        if (from.empty()) {
            return;
        }

        size_t start_pos = 0;
        while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length();
        }
    }

    /**
     * Convert new-lines to <br/>
     */
    static inline void nl2br(std::string &s)
    {
        for (size_t pos = 0; pos < s.length(); ++pos) {
            char c = s[pos];
            if (c == '\r' || c == '\n') {
                if (pos + 1 < s.length()) {
                    // Do a read-ahead for a second char
                    char next = s[pos + 1];
                    if ((c == '\r' && next == '\n') || (c == '\n' && next == '\r')) {
                        s.replace(pos, 2, "<br/>");
                        continue;
                    }
                }

                s.replace(pos, 1, "<br/>");
            }
        }
    }

    /**
     * Convert new-lines to <p>...</p>
     */
    static inline void nl2p(std::string &s)
    {
        for (size_t pos = 0; pos < s.length(); ++pos) {
            char c = s[pos];
            if (c == '\r' || c == '\n') {
                if (pos + 1 < s.length()) {
                    // Do a read-ahead for a second char
                    char next = s[pos + 1];
                    if ((c == '\r' && next == '\n') || (c == '\n' && next == '\r')) {
                        s.replace(pos, 2, "</p><p>");
                        continue;
                    }
                }

                s.replace(pos, 1, "</p><p>");
            }
        }

        s.insert(0, "<p>");
        s.insert(s.length(), "</p>");
    }
};

}  // namespace bes::templating
