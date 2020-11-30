#include "web_responder.h"

using namespace bes::web;

/**
 * Wrap the FastCGI entry-point so that we can control error templates.
 */
int WebResponder::Run()
{
    // For stats only
    auto t_start = std::chrono::high_resolution_clock::now();
    std::string method = "-";
    std::string uri = "-";
    std::string ret_status = "-";

    try {
        HttpRequest http_req(request);
        method = request.Param(Http::Parameter::REQUEST_METHOD);
        uri = request.Param(Http::Parameter::REQUEST_URI);

        // There are a few layers of error handling here to try to offer the best error response to the client
        try {
            /// Normal response handling
            bool responded = false;
            for (auto const& router : *(request.container.Get<std::vector<std::shared_ptr<Router>>>(SVC_ROUTER))) {
                try {
                    // Get an HttpResponse from the router, if applicable
                    auto resp = router->YieldResponse(http_req);

                    // Validate it has a response code
                    try {
                        ret_status = resp.Headers().at(Http::Header::STATUS);
                    } catch (...) {
                        ret_status = "200";
                        resp.Status(Http::Status::OK);
                    }

                    // Render, break from the loop
                    RenderResponse(resp, http_req);
                    responded = true;
                    break;
                } catch (CannotYieldException const&) {
                    // Router couldn't yield a response, continue looking
                }
            }

            // No routers could yield a response, raise a 404 (this is acceptable behavior)
            if (!responded) {
                throw NotFoundHttpException();
            }

        } catch (RedirectHttpException const& e) {
            /// Redirect response handling
            HttpResponse resp;
            resp.Status(e.HttpCode());
            resp.Header(Http::Header::LOCATION, e.Target());
            ret_status = resp.Headers().at(Http::Header::STATUS);
            RenderResponse(resp, http_req);
        } catch (HttpWebException const& e) {
            /// HTTP error handling
            ret_status = std::to_string(static_cast<int>(e.HttpCode()));
            RenderError(http_req, e.HttpCode(), e.ErrorMessage());
        } catch (std::exception const& e) {
            /// Other exceptions - internal server error
            ret_status = "500";
            RenderError(http_req, Http::Status::INTERNAL_SERVER_ERROR, e.what());
        }

    } catch (std::exception& e) {
        /// Error creating request object or an exception was thrown in an exception handler.
        // We can't render a normal error message without a request object so we'll fall-back to emergency error
        // rendering.
        ret_status = "500";
        if (request.HasParam(Http::Parameter::REQUEST_URI)) {
            BES_LOG(ERROR) << "Fallback exception handling: " << request.Param(Http::Parameter::REQUEST_URI)
                           << " exception: " << e.what();
        } else {
            BES_LOG(ERROR) << "Fallback exception handling with missing REQUEST_URI: " << e.what()
                           << ", parameter dump follows:";
            for (auto const& p : request.Params()) {
                BES_LOG(ERROR) << " * " << p.first << ": " << p.second;
            }
        }
        RenderEmergencyErrorResponse(e.what());
    }

    auto t_end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t_end - t_start).count();

    BES_LOG(INFO) << "[HTTP] " << method << " " << uri << " -> " << ret_status << " in " << duration << " μs";

    return 0;
}

void WebResponder::RenderError(HttpRequest const& req, Http::Status code, std::string const& debug_msg)
{
    for (auto const& router : *(request.container.Get<std::vector<std::shared_ptr<Router>>>("routers"))) {
        try {
            // Render error response, if this router yields one
            RenderResponse(router->YieldErrorResponse(req, code, debug_msg), req);
        } catch (CannotYieldException const&) {
            // Router does not handle this error type, continue looking
            // At least one router should be a catch-all error handler
        }
    }

    // No routers could handle this error, this is not acceptable as at least one router should handle ALL errors,
    // forcing us to fall-back to an emergency error handler
    RenderEmergencyErrorResponse("No routers contain a handler for this error: " + debug_msg);
}

void WebResponder::RenderResponse(HttpResponse const& resp, HttpRequest const& req)
{
    // Render headers
    for (auto const& header : resp.Headers()) {
        out << header.first << ": " << header.second << "\n";
    }
    // Cookies
    for (auto const& it : resp.Cookies()) {
        RenderCookie(it.second);
    }

    // Check for a session, add a session cookie if we have a session
    if (req.HasSession()) {
        Cookie session_cookie(*(request.container.Get<std::string>(SESSION_COOKIE_KEY)), req.GetSession().SessionId());
        session_cookie.HttpOnly(true);
        if (*(request.container.Get<bool>(SESSION_SECURE_KEY))) {
            session_cookie.Secure(true);
        }

        RenderCookie(session_cookie);
    }

    // End of headers
    out << "\n";

    // Render content
    out << resp.Content();
}

void WebResponder::RenderCookie(Cookie const& cookie)
{
    out << "Set-Cookie: " << cookie.Name() << "=" << cookie.Value();

    if (!cookie.Domain().empty()) {
        out << "; Domain=" << cookie.Domain();
    }

    if (!cookie.Path().empty()) {
        out << "; Path=" << cookie.Path();
    }

    if (cookie.MaxAge()) {
        out << "; Max-Age=" << std::to_string(cookie.MaxAge());
    } else if (cookie.Expires() > std::chrono::system_clock::now()) {
        std::time_t exp_time_t = std::chrono::system_clock::to_time_t(cookie.Expires());
        std::tm exp_tm = *std::localtime(&exp_time_t);
        char buf[40];  // Should be ~ 30 chars
        strftime(buf, 40, "%a, %d-%b-%Y %T GMT", &exp_tm);
        out << "; Expires=" << buf;
    }

    if (cookie.Secure()) {
        out << "; Secure";
    }

    if (cookie.HttpOnly()) {
        out << "; HttpOnly";
    }

    out << "\n";
}

void WebResponder::RenderEmergencyErrorResponse(std::string const& debug_msg)
{
    out << Http::Header::STATUS << ": " << static_cast<int>(Http::Status::INTERNAL_SERVER_ERROR) << "\n";
    out << Http::Header::CONTENT_TYPE << ": " << Http::ContentType::HTML << "\n\n";

    out << "<html><head><title>Internal Server Error</title></head><body><h1>Internal Server Error</h1>";

    if (debug_msg.length() && DebugMode()) {
        out << "<p>" << debug_msg << "</p>";
    }

    out << "</body></html>";
}

/**
 * Checks if the request container was loaded with an option allowing us to render debug errors
 */
bool WebResponder::DebugMode()
{
    try {
        return *(request.container.Get<bool>(DEBUG_KEY));
    } catch (...) {
        return false;
    }
}
