#ifndef BES_WEB_TEMPLATING_TEMPLATING_CONTROLLER_H
#define BES_WEB_TEMPLATING_TEMPLATING_CONTROLLER_H

#include <bes/templating.h>
#include <bes/web.h>

#include <memory>
#include <nlohmann/json.hpp>
#include <unordered_map>

namespace bes::web {

using Ctx = bes::templating::data::ContextBuilder;
using json = nlohmann::json;

class TemplatingController
{
   public:
    explicit TemplatingController(std::shared_ptr<bes::templating::Engine>);

   protected:
    [[nodiscard]] HttpResponse Response(std::string const& templ,
                                        bes::templating::data::ContextBuilder const& ctx) const;

    [[nodiscard]] HttpResponse Response(std::string const& templ, bes::templating::data::ContextBuilder const& ctx,
                                        HttpResponse&& resp) const;

    [[nodiscard]] HttpResponse Response(std::string const& templ, bes::templating::data::ContextBuilder const& ctx,
                                        bes::web::Http::Status status) const;

    [[nodiscard]] HttpResponse Response(std::string const& templ, bes::templating::data::ContextBuilder const& ctx,
                                        std::string const& content_type) const;

    [[nodiscard]] HttpResponse Response(std::string const& templ, bes::templating::data::ContextBuilder const& ctx,
                                        bes::web::Http::Status status, std::string const& content_type) const;

    [[nodiscard]] HttpResponse JsonResponse(json const& j) const;

    [[nodiscard]] HttpResponse JsonResponse(json const& j, bes::web::Http::Status status) const;

    std::shared_ptr<bes::templating::Engine> renderer;
};

}  // namespace bes::web

#endif
