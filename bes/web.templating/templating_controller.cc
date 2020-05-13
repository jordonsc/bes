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
                                            bes::web::Http::Status status)
{
    return Response(templ, ctx, status, bes::web::Http::ContentType::HTML);
}

HttpResponse TemplatingController::Response(std::string const& templ, bes::templating::data::ContextBuilder const& ctx,
                                            bes::web::Http::Status status, std::string const& content_type)
{
    HttpResponse resp;

    // Min headers
    resp.Status(status, content_type);

    // Content body & content-length header
    resp.Header(bes::web::Http::Header::CONTENT_LENGTH,
                std::to_string(resp.Write(renderer->Render(templ, ctx.GetContext()))));

    return resp;
}

HttpResponse TemplatingController::Response(std::string const& templ, bes::templating::data::ContextBuilder const& ctx,
                                            std::string const& content_type)
{
    return Response(templ, ctx, bes::web::Http::Status::OK, content_type);
}

HttpResponse TemplatingController::Response(std::string const& templ, bes::templating::data::ContextBuilder const& ctx,
                                            bes::web::Http::Status status, std::string const& content_type,
                                            std::unordered_map<std::string, std::string> const& headers)
{
    HttpResponse resp = Response(templ, ctx, status, content_type);

    // Add headers to the response object
    for (auto const& hdr : headers) {
        resp.Header(hdr.first, hdr.second);
    }

    return resp;
}
