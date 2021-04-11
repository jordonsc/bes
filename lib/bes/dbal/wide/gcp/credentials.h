#ifndef BES_DBAL_WIDE_GCP_CREDENTIALS_H
#define BES_DBAL_WIDE_GCP_CREDENTIALS_H

#include <string>

namespace bes::dbal::wide::bigtable {

class Credentials {
   public:
    std::string project_id;
    std::string instance_id;
};

}  // namespace bes::dbal

#endif
