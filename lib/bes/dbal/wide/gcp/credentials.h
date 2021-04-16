#pragma once

#include <string>

namespace bes::dbal::wide::bigtable {

class Credentials {
   public:
    std::string project_id;
    std::string instance_id;
};

}  // namespace bes::dbal
