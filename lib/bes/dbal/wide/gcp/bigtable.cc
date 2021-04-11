#include "bigtable.h"

using namespace bes::dbal::wide;

BigTable::BigTable(bigtable::Credentials credentials) : credentials(std::move(credentials)) {}
