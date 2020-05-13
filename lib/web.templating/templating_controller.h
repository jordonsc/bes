#ifndef BES_WEB_TEMPLATING_TEMPLATING_CONTROLLER_H
#define BES_WEB_TEMPLATING_TEMPLATING_CONTROLLER_H

#include <memory>
#include <unordered_map>

#include "lib/templating/templating.h"
#include "lib/web/web.h"

namespace bes::web {

using Ctx = bes::templating::data::ContextBuilder;

class TemplatingController
{
   public:
    TemplatingController(std::shared_ptr<bes::templating::Engine> const&);

   protected:
    HttpResponse Response(std::string const& templ, bes::templating::data::ContextBuilder const& ctx);

    HttpResponse Response(std::string const& templ, bes::templating::data::ContextBuilder const& ctx,
                          bes::web::Http::Status status);

    HttpResponse Response(std::string const& templ, bes::templating::data::ContextBuilder const& ctx,
                          std::string const& content_type);

    HttpResponse Response(std::string const& templ, bes::templating::data::ContextBuilder const& ctx,
                          bes::web::Http::Status status, std::string const& content_type);

    HttpResponse Response(std::string const& templ, bes::templating::data::ContextBuilder const& ctx,
                          bes::web::Http::Status status, std::string const& content_type,
                          std::unordered_map<std::string, std::string> const& headers);

    std::shared_ptr<bes::templating::Engine> renderer;
};

}  // namespace bes::web

#endif
