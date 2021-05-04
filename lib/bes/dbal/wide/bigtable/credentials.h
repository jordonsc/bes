#pragma once

#include <string>
#include <utility>

#include "../../context.h"

namespace bes::dbal::wide::bigtable {

class Credentials
{
   public:
    Credentials() = default;
    Credentials(std::string pid, std::string iid) : project_id(std::move(pid)), instance_id(std::move(iid)) {}

    explicit Credentials(bes::dbal::Context const& ctx)
    {
        try {
            project_id = ctx.getParameter("project-id");
            instance_id = ctx.getParameter("instance-id");
        } catch (bes::dbal::OutOfRangeException const&) {
            throw bes::dbal::DbalException("BigTable database requires context object 'project-id' and 'instance-id'");
        }
    }

    std::string project_id;
    std::string instance_id;
};

}  // namespace bes::dbal::wide::bigtable
