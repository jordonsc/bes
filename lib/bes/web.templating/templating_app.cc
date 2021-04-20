#include "templating_app.h"

using namespace bes::web;

TemplateApp::TemplateApp(std::string name, std::string key, std::string description, bes::version_t version,
                         std::string usage)
    : Application(std::move(name), std::move(key), std::move(description), std::move(version), std::move(usage))
{
    // Add core services to kernel container
    kernel().getContainer().emplace<bes::templating::Engine>("templating");
    kernel().getContainer().emplace<bes::web::MappedRouter>("router");
}

void TemplateApp::bootstrap()
{
    if (bootstrapped) {
        return;
    }

    bootstrapped = true;

    // Build version, template schema and route schema can all be defined in the config or from CLI
    build = kernel().getConfig().getOr<std::string>("static", "web", "build");
    auto tmpl_schema = kernel().getConfig().getOr<std::string>("/app/templating.yml", "web", "templating");
    auto route_schema = kernel().getConfig().getOr<std::string>("/app/routing.yml", "web", "routing");

    debug_mode = kernel().getCli()["debug"].present();
    if (debug_mode) {
        BES_LOG(WARNING) << "Debug mode enabled; web server will render error details";
    }

    auto const& cli_build = kernel().getCli()["build"];
    if (cli_build.present()) {
        build = cli_build.as<std::string>();
    }

    auto const& cli_templates = kernel().getCli()["templating"];
    if (cli_templates.present()) {
        tmpl_schema = cli_templates.as<std::string>();
    }

    auto const& cli_routing = kernel().getCli()["routing"];
    if (cli_routing.present()) {
        route_schema = cli_routing.as<std::string>();
    }

    // Parse the templating YAML file and load templates
    BES_LOG(DEBUG) << "Parsing template schema: " << tmpl_schema;
    try {
        loadTemplates(tmpl_schema);
    } catch (YAML::BadFile const&) {
        BES_LOG(ERROR) << "Bad template schema file: " << tmpl_schema;
    }

    // Router
    BES_LOG(DEBUG) << "Parsing route schema: " << route_schema;
    auto router = kernel().getContainer().get<bes::web::MappedRouter>("router");
    try {
        router->loadRoutesFromFile(route_schema);
    } catch (YAML::BadFile const&) {
        BES_LOG(ERROR) << "Bad routing schema file: " << route_schema;
    }

    // Controllers
    RegisterControllers(*router);
}

void TemplateApp::loadTemplates(std::string const& fn)
{
    auto templating = kernel().getContainer().get<bes::templating::Engine>("templating");
    YAML::Node root = YAML::LoadFile(fn);

    // Search paths
    auto search = root["search"];
    if (search.IsDefined()) {
        if (!search.IsSequence()) {
            throw bes::web::WebException("'search' node in template schema must be a string list");
        }

        for (auto const& node : search) {
            if (!node.IsScalar()) {
                throw bes::web::WebException("'search' list contains non-scalar elements");
            }

            auto sp = node.as<std::string>();
            templating->search_path.AppendSearchPath(sp);
            BES_LOG(DEBUG) << "Registered template path: " << sp;
        }
    }

    templating->search_path.AppendSearchPath(".");

    auto templates = root["templates"];
    if (templates.IsDefined()) {
        if (!templates.IsSequence()) {
            throw bes::web::WebException("'templates' node must be a list of name/filename pairs");
        }

        for (auto const& node : templates) {
            if (!node.IsMap()) {
                throw bes::web::WebException("'templates' list contains non-map elements");
            }

            auto node_name = node["name"];
            auto node_file = node["filename"];

            if (!node_name.IsDefined() || !node_file.IsDefined() || !node_name.IsScalar() || !node_file.IsScalar()) {
                throw bes::web::WebException("'templates' must contain string 'name' and 'filename' nodes");
            }

            try {
                templating->loadFile(node_name.as<std::string>(), node_file.as<std::string>());
            } catch (bes::FileNotFoundException const&) {
                BES_LOG(ERROR) << "Unable to load template '" << node_name << "': cannot locate file '" << node_file
                               << "'";
            }
        }
    }
}

void TemplateApp::configureCli(bes::cli::Parser& parser)
{
    Application::configureCli(parser);

    parser << bes::cli::Arg('b', "build", bes::cli::ValueType::REQUIRED)
           << bes::cli::Arg('t', "templating", bes::cli::ValueType::REQUIRED)
           << bes::cli::Arg('d', "debug", bes::cli::ValueType::NONE)
           << bes::cli::Arg('r', "routing", bes::cli::ValueType::REQUIRED);
}

void TemplateApp::run()
{
    bootstrap();
    BES_LOG(INFO) << "Build: " << build;

    // Start the FastCGI server
    svc = std::make_unique<bes::web::WebServer>();
    svc->addRouter(kernel().getContainer().get<bes::web::MappedRouter>("router"));

    // Allow the app to add a session manager or other configuration
    ConfigureServer(*(svc.get()));

    auto session_cookie = kernel().getConfig().getOr<std::string>("bsn", "web", "sessions", "cookie");
    auto session_prefix = kernel().getConfig().getOr<std::string>("S", "web", "sessions", "prefix");

    svc->run(bes::net::Address(kernel().getConfig().getOr<std::string>("0.0.0.0", "server", "bind"),
                               kernel().getConfig().getOr<uint16_t>(9000, "server", "listen")),
             debug_mode);
}

void TemplateApp::shutdown()
{
    BES_LOG(INFO) << "Shutting down FastCGI interface..";
    svc->shutdown();
    BES_LOG(INFO) << "Shutting down application..";
    Application::shutdown();
}