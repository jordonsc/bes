#include "templating_controller.h"

#include <utility>

using namespace bes::web;

TemplatingController::TemplatingController(std::shared_ptr<bes::templating::Engine> renderer)
    : renderer(std::move(renderer))
{}

HttpResponse TemplatingController::Response(std::string const& templ,
                                            bes::templating::data::ContextBuilder const& ctx) const
{
    return Response(templ, ctx, bes::web::Http::Status::OK, bes::web::Http::ContentType::HTML);
}

HttpResponse TemplatingController::Response(std::string const& templ, bes::templating::data::ContextBuilder const& ctx,
                                            HttpResponse&& resp) const
{
    resp.Header(bes::web::Http::Header::CONTENT_LENGTH,
                std::to_string(resp.Write(renderer->Render(templ, ctx.GetContext()))));

    return HttpResponse{std::move(resp)};
}

HttpResponse TemplatingController::Response(std::string const& templ, bes::templating::data::ContextBuilder const& ctx,
                                            bes::web::Http::Status status) const
{
    return Response(templ, ctx, status, bes::web::Http::ContentType::HTML);
}

HttpResponse TemplatingController::Response(std::string const& templ, const bes::templating::data::ContextBuilder& ctx,
                                            std::string const& content_type) const
{
    return Response(templ, ctx, bes::web::Http::Status::OK, content_type);
}

HttpResponse TemplatingController::Response(std::string const& templ, bes::templating::data::ContextBuilder const& ctx,
                                            bes::web::Http::Status status, std::string const& content_type) const
{
    HttpResponse resp;
    resp.Status(status, content_type);
    resp.Header(bes::web::Http::Header::CONTENT_LENGTH,
                std::to_string(resp.Write(renderer->Render(templ, ctx.GetContext()))));

    return resp;
}

HttpResponse TemplatingController::JsonResponse(json const& j) const
{
    return JsonResponse(j, bes::web::Http::Status::OK);
}

HttpResponse TemplatingController::JsonResponse(json const& j, bes::web::Http::Status status) const
{
    HttpResponse resp;
    resp.Status(status, bes::web::Http::ContentType::JSON);
    resp.Header(bes::web::Http::Header::CONTENT_LENGTH, std::to_string(resp.Write(j.dump())));

    return resp;
}
