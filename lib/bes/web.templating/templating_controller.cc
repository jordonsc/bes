#include "templating_controller.h"

#include <utility>

using namespace bes::web;

TemplatingController::TemplatingController(std::shared_ptr<bes::templating::Engine> renderer)
    : renderer(std::move(renderer))
{}

HttpResponse TemplatingController::response(std::string const& templ,
                                            bes::templating::data::ContextBuilder const& ctx) const
{
    return response(templ, ctx, bes::web::Http::Status::OK, bes::web::Http::ContentType::HTML);
}

HttpResponse TemplatingController::response(std::string const& templ, bes::templating::data::ContextBuilder const& ctx,
                                            HttpResponse&& resp) const
{
    resp.header(bes::web::Http::Header::CONTENT_LENGTH,
                std::to_string(resp.write(renderer->render(templ, ctx.getContext()))));

    return HttpResponse{std::move(resp)};
}

HttpResponse TemplatingController::response(std::string const& templ, bes::templating::data::ContextBuilder const& ctx,
                                            bes::web::Http::Status status) const
{
    return response(templ, ctx, status, bes::web::Http::ContentType::HTML);
}

HttpResponse TemplatingController::response(std::string const& templ, const bes::templating::data::ContextBuilder& ctx,
                                            std::string const& content_type) const
{
    return response(templ, ctx, bes::web::Http::Status::OK, content_type);
}

HttpResponse TemplatingController::response(std::string const& templ, bes::templating::data::ContextBuilder const& ctx,
                                            bes::web::Http::Status status, std::string const& content_type) const
{
    HttpResponse resp;
    resp.status(status, content_type);
    resp.header(bes::web::Http::Header::CONTENT_LENGTH,
                std::to_string(resp.write(renderer->render(templ, ctx.getContext()))));

    return resp;
}

HttpResponse TemplatingController::jsonResponse(json const& j) const
{
    return jsonResponse(j, bes::web::Http::Status::OK);
}

HttpResponse TemplatingController::jsonResponse(json const& j, bes::web::Http::Status status) const
{
    HttpResponse resp;
    resp.status(status, bes::web::Http::ContentType::JSON);
    resp.header(bes::web::Http::Header::CONTENT_LENGTH, std::to_string(resp.write(j.dump())));

    return resp;
}
