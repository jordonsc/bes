#pragma once

#include <yaml-cpp/yaml.h>

#include <bes/app.h>
#include <bes/templating.h>
#include <bes/web.h>

namespace bes::web {

class TemplateApp : public bes::app::Application
{
   public:
    TemplateApp(std::string name, std::string key, std::string description, version_t version, std::string usage = "");

    void run() override;
    void shutdown() override;
    void configureCli(bes::cli::Parser& parser) override;

   protected:
    virtual void configureServer(bes::web::WebServer& svr) = 0;
    virtual void registerControllers(bes::web::MappedRouter&) = 0;

    std::unique_ptr<bes::web::WebServer> svc;
    std::string build;
    bool debug_mode = false;

   private:
    void bootstrap();
    static void loadTemplates(std::string const& fn);

    bool bootstrapped = false;
};

}  // namespace bes::web
