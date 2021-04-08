#include "bigtable.h"

using namespace bes::dbal;

BigTable::BigTable(bigtable::Credentials credentials) : credentials(std::move(credentials)) {}
