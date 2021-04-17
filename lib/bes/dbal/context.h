#ifndef BES_DBAL_CONTEXT_H
#define BES_DBAL_CONTEXT_H

#include <string>
#include <unordered_map>

namespace bes::dbal {

class Context
{
   public:
    Context() = default;
    explicit Context(std::unordered_map<std::string, std::string> params);

    [[nodiscard]] bool hasParameter(std::string const&) const;
    [[nodiscard]] std::string const& getParameter(std::string const&) const;
    [[nodiscard]] std::string const& getOr(std::string const&, std::string const&) const;
    void setParameter(std::string const& key, std::string value);

   protected:
    std::unordered_map<std::string, std::string> params;
};

}  // namespace bes::dbal

#endif
