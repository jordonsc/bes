#ifndef BES_WEB_TEMPLATING_TEMPLATING_APP_H
#define BES_WEB_TEMPLATING_TEMPLATING_APP_H

#include <yaml-cpp/yaml.h>

#include "bes/app.h"
#include "bes/templating.h"
#include "bes/web.h"

namespace bes::web {

class TemplateApp : public bes::app::Application
{
   public:
    TemplateApp(std::string name, std::string key, std::string description, version_t version, std::string usage = "");

    void Run() override;
    void Shutdown() override;
    void ConfigureCli(bes::cli::Parser& parser) override;

   protected:
    virtual void ConfigureServer(bes::web::WebServer& svr) = 0;
    virtual void RegisterControllers(bes::web::MappedRouter&) = 0;

    std::unique_ptr<bes::web::WebServer> svc;
    std::string build;
    bool debug_mode = false;

   private:
    void Bootstrap();
    static void LoadTemplates(std::string const& fn);

    bool bootstrapped = false;
};

}  // namespace bes::web

#endif
