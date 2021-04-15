#ifndef BES_DBAL_WIDE_CASSANDRA_RESULT_H
#define BES_DBAL_WIDE_CASSANDRA_RESULT_H

#include "result_iterator.h"

namespace bes::dbal::wide::cassandra {

class Result
{
   public:
    Result() = delete;
    explicit Result(CassResult const* r);

    [[nodiscard]] ResultIterator begin() const;
    [[nodiscard]] ResultIterator end() const;

    [[nodiscard]] size_t RowCount() const;

   private:
    std::shared_ptr<CassResult> result = nullptr;
};

}  // namespace bes::dbal::wide::cassandra

#endif
