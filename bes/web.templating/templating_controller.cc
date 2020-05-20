#include "templating_controller.h"

#include <utility>

using namespace bes::web;

TemplatingController::TemplatingController(std::shared_ptr<bes::templating::Engine> renderer)
    : renderer(std::move(renderer))
{}

HttpResponse TemplatingController::Response(std::string const& templ, bes::templating::data::ContextBuilder const& ctx)
{
    return Response(templ, ctx, bes::web::Http::Status::OK, bes::web::Http::ContentType::HTML);
}

HttpResponse TemplatingController::Response(std::string const& templ, bes::templating::data::ContextBuilder const& ctx,
                                            HttpResponse&& resp)
{
    resp.Header(bes::web::Http::Header::CONTENT_LENGTH,
                std::to_string(resp.Write(renderer->Render(templ, ctx.GetContext()))));

    return HttpResponse{std::move(resp)};
}

HttpResponse TemplatingController::Response(std::string const& templ, bes::templating::data::ContextBuilder const& ctx,
                                            bes::web::Http::Status status)
{
    return Response(templ, ctx, status, bes::web::Http::ContentType::HTML);
}

HttpResponse TemplatingController::Response(std::string const& templ, bes::templating::data::ContextBuilder const& ctx,
                                            bes::web::Http::Status status, std::string const& content_type)
{
    HttpResponse resp;
    resp.Status(status, content_type);
    resp.Header(bes::web::Http::Header::CONTENT_LENGTH,
                std::to_string(resp.Write(renderer->Render(templ, ctx.GetContext()))));

    return resp;
}
