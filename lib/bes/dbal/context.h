#ifndef BES_DBAL_CONTEXT_H
#define BES_DBAL_CONTEXT_H

#include <string>
#include <unordered_map>

namespace bes::dbal {

class Context
{
   public:
    [[nodiscard]] bool HasParameter(std::string const&) const;
    [[nodiscard]] std::string const& GetParameter(std::string const&) const;
    [[nodiscard]] std::string const& GetOr(std::string const&, std::string const&) const;
    void SetParameter(std::string const& key, std::string value);

   protected:
    std::unordered_map<std::string, std::string> params;
};

}  // namespace bes::dbal

#endif
