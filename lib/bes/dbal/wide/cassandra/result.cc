#include "result.h"

using namespace bes::dbal::wide::cassandra;

Result::Result(CassResult const* r)
{
    result.reset(const_cast<CassResult*>(r), [](CassResult* item) {
        cass_result_free(item);
    });
}

ResultIterator Result::begin() const
{
    return ResultIterator(result.get());
}

ResultIterator Result::end() const
{
    return ResultIterator();
}

size_t Result::RowCount() const
{
    return cass_result_row_count(result.get());
}
