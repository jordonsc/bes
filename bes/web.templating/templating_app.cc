#include "templating_app.h"

using namespace bes::web;

TemplateApp::TemplateApp(std::string name, std::string key, std::string description, bes::version_t version,
                         std::string usage)
    : Application(std::move(name), std::move(key), std::move(description), std::move(version), std::move(usage))
{
    // Add core services to kernel container
    Kernel().Container().Emplace<bes::templating::Engine>("templating");
    Kernel().Container().Emplace<bes::web::MappedRouter>("router");
}

void TemplateApp::Bootstrap()
{
    if (bootstrapped) {
        return;
    }

    bootstrapped = true;

    // Build version, template schema and route schema can all be defined in the config or from CLI
    build = Kernel().Config().GetOr<std::string>("static", "web", "build");
    auto tmpl_schema = Kernel().Config().GetOr<std::string>("/app/templating.yml", "web", "templating");
    auto route_schema = Kernel().Config().GetOr<std::string>("/app/routing.yml", "web", "routing");

    auto const& cli_build = Kernel().Cli()["build"];
    if (cli_build.Present()) {
        build = cli_build.as<std::string>();
    }

    auto const& cli_templates = Kernel().Cli()["templating"];
    if (cli_templates.Present()) {
        tmpl_schema = cli_templates.as<std::string>();
    }

    auto const& cli_routing = Kernel().Cli()["routing"];
    if (cli_routing.Present()) {
        route_schema = cli_routing.as<std::string>();
    }

    // Parse the templating YAML file and load templates
    BES_LOG(DEBUG) << "Parsing template schema: " << tmpl_schema;
    try {
        LoadTemplates(tmpl_schema);
    } catch (YAML::BadFile const&) {
        BES_LOG(ERROR) << "Bad template schema file: " << tmpl_schema;
    }

    // Router
    BES_LOG(DEBUG) << "Parsing route schema: " << route_schema;
    auto router = Kernel().Container().Get<bes::web::MappedRouter>("router");
    try {
        router->LoadRoutesFromFile(route_schema);
    } catch (YAML::BadFile const&) {
        BES_LOG(ERROR) << "Bad routing schema file: " << route_schema;
    }

    // Controllers
    RegisterControllers(*router);
}

void TemplateApp::LoadTemplates(std::string const& fn)
{
    auto templating = Kernel().Container().Get<bes::templating::Engine>("templating");
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
                templating->LoadFile(node_name.as<std::string>(), node_file.as<std::string>());
            } catch (bes::FileNotFoundException const&) {
                BES_LOG(ERROR) << "Unable to load template '" << node_name << "': cannot locate file '" << node_file
                               << "'";
            }
        }
    }
}

void TemplateApp::ConfigureCli(bes::cli::Parser& parser)
{
    Application::ConfigureCli(parser);

    parser << bes::cli::Arg('b', "build", bes::cli::ValueType::REQUIRED)
           << bes::cli::Arg('t', "templating", bes::cli::ValueType::REQUIRED)
           << bes::cli::Arg('r', "routing", bes::cli::ValueType::REQUIRED);
}

void TemplateApp::Run()
{
    Bootstrap();
    BES_LOG(INFO) << "Build: " << build;

    // Start the FastCGI server
    BES_LOG(INFO) << "Starting FastCGI server..";
    svc = std::make_unique<bes::web::WebServer>();
    svc->AddRouter(Kernel().Container().Get<bes::web::MappedRouter>("router"));
    svc->Run(bes::net::Address(Kernel().Config().GetOr<std::string>("0.0.0.0", "server", "bind"),
                               Kernel().Config().GetOr<uint16_t>(9000, "server", "listen")),
             true);
}

void TemplateApp::Shutdown()
{
    BES_LOG(INFO) << "Shutting down FastCGI interface..";
    svc->Shutdown();
    BES_LOG(INFO) << "Shutting down application..";
    Application::Shutdown();
}