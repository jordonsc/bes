#ifndef BES_WEB_TEMPLATING_TEMPLATING_CONTROLLER_H
#define BES_WEB_TEMPLATING_TEMPLATING_CONTROLLER_H

#include <memory>
#include <unordered_map>

#include <bes/templating.h>
#include <bes/web.h>

namespace bes::web {

using Ctx = bes::templating::data::ContextBuilder;

class TemplatingController
{
   public:
    explicit TemplatingController(std::shared_ptr<bes::templating::Engine>);

   protected:
    HttpResponse Response(std::string const& templ, bes::templating::data::ContextBuilder const& ctx);

    HttpResponse Response(std::string const& templ, bes::templating::data::ContextBuilder const& ctx,
                          HttpResponse &&resp);

    HttpResponse Response(std::string const& templ, bes::templating::data::ContextBuilder const& ctx,
                          bes::web::Http::Status status);

    HttpResponse Response(std::string const& templ, bes::templating::data::ContextBuilder const& ctx,
                          std::string const& content_type);

    HttpResponse Response(std::string const& templ, bes::templating::data::ContextBuilder const& ctx,
                          bes::web::Http::Status status, std::string const& content_type);

    std::shared_ptr<bes::templating::Engine> renderer;
};

}  // namespace bes::web

#endif
