#pragma once

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
    [[nodiscard]] HttpResponse response(std::string const& templ,
                                        bes::templating::data::ContextBuilder const& ctx) const;

    [[nodiscard]] HttpResponse response(std::string const& templ, bes::templating::data::ContextBuilder const& ctx,
                                        HttpResponse&& resp) const;

    [[nodiscard]] HttpResponse response(std::string const& templ, bes::templating::data::ContextBuilder const& ctx,
                                        bes::web::Http::Status status) const;

    [[nodiscard]] HttpResponse response(std::string const& templ, bes::templating::data::ContextBuilder const& ctx,
                                        std::string const& content_type) const;

    [[nodiscard]] HttpResponse response(std::string const& templ, bes::templating::data::ContextBuilder const& ctx,
                                        bes::web::Http::Status status, std::string const& content_type) const;

    [[nodiscard]] HttpResponse jsonResponse(json const& j) const;

    [[nodiscard]] HttpResponse jsonResponse(json const& j, bes::web::Http::Status status) const;

    std::shared_ptr<bes::templating::Engine> renderer;
};

}  // namespace bes::web
