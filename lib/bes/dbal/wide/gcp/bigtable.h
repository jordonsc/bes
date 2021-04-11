#ifndef BES_DBAL_WIDE_GCP_BIGTABLE_H
#define BES_DBAL_WIDE_GCP_BIGTABLE_H

#include "../../exception.h"
#include "../wide_column_db.h"
#include "credentials.h"
#include "google/cloud/bigtable/table.h"

namespace bes::dbal::wide {

/**
 * Consider: https://cloud.google.com/bigtable/docs/schema-design
 */
class BigTable : public WideColumnDb
{
   public:
    explicit BigTable(bigtable::Credentials credentials);

    void CreateTable(std::string const& table_name) {
        google::bigtable::v2::Cell cell;


    }

   protected:
    bigtable::Credentials credentials;
};

}  // namespace bes::dbal::wide

#endif
