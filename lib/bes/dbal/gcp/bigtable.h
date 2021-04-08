#ifndef BES_DBAL_GCP_BIGTABLE_H
#define BES_DBAL_GCP_BIGTABLE_H

#include "../exception.h"
#include "../wide.h"
#include "credentials.h"

namespace bes::dbal {

class BigTable : public WideColumnDb
{
   public:
    explicit BigTable(bigtable::Credentials credentials);

    void CreateTable(std::string const& table_name) {}

   protected:
    bigtable::Credentials credentials;
};

}  // namespace bes::dbal

#endif
