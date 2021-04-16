#ifndef BES_DBAL_WIDE_CASSANDRA_QUERY_H
#define BES_DBAL_WIDE_CASSANDRA_QUERY_H

#include <string>
#include <vector>

#include "../../exception.h"
#include "../cell.tcc"
#include "../result.tcc"
#include "cassandra.h"
#include "result_iterator.h"
#include "row_iterator.h"
#include "utility.h"

namespace bes::dbal::wide {
class Cassandra;

template <>
inline size_t Result<cassandra::ResultIterator, std::shared_ptr<CassResult>>::rowCount() const
{
    return cass_result_row_count(data.get());
}

template <>
inline Cell Row<cassandra::RowIterator, CassRow const*>::operator[](size_t index)
{
    return cassandra::Utility::createCellFromColumn(cass_row_get_column(data, index));
}

}  // namespace bes::dbal::wide

namespace bes::dbal::wide::cassandra {

/**
 * RAII query container.
 *
 * Not thread safe.
 */
class Query
{
   public:
    explicit Query(std::string cql, size_t num_params = 0);
    virtual ~Query();

    template <class T>
    void bind(T v);

   private:
    enum class ExecMode
    {
        PENDING,
        EXEC_SYNC,
        EXEC_ASYNC,
        COMPLETE,
    };

    /**
     * Throws a DbalException if the query is not in the correct state to execute.
     */
    void execValidation() const;

    /**
     * Runs the query against a connection.
     */
    void executeSync(Connection const& con);

    /**
     * Executes and returns immediately.
     */
    void executeAsync(Connection const& con);

    /**
     * Get an iterable result object, which can be used to return row data.
     *
     * If called with a connection, it will assume a synchronous single-call transaction and execute then return the
     * result.
     *
     * If called without parameters, it will assume you've first called ExecAsync() and throw an exception if you have
     * not.
     */
    [[nodiscard]] ResultT getResult();
    [[nodiscard]] ResultT getResult(Connection const& con);

    /**
     * Waits for query to complete.
     */
    void wait();

    std::string cql;
    std::vector<std::string> str_cache;
    CassFuture* future;
    CassStatement* statement;
    size_t q_pos = 0;
    size_t expected_params = 0;
    ExecMode mode = ExecMode::PENDING;

    friend class ::bes::dbal::wide::Cassandra;
};

// --------------------------------- //

Query::Query(std::string cql, size_t num_params) : cql(std::move(cql)), future(nullptr), expected_params(num_params)
{
    statement = cass_statement_new(this->cql.c_str(), num_params);
}

Query::~Query()
{
    if (future != nullptr) {
        cass_future_free(future);
    }

    if (statement != nullptr) {
        cass_statement_free(statement);
    }
}

void Query::executeSync(Connection const& con)
{
    execValidation();

    try {
        mode = ExecMode::EXEC_SYNC;
        future = cass_session_execute(con.getSessionPtr(), statement);
        cass_future_wait(future);
        mode = ExecMode::COMPLETE;

        CassError rc = cass_future_error_code(future);
        if (rc != CASS_OK) {
            throw DbalException(Utility::getFutureErrMsg(future));
        }

    } catch (std::exception const& e) {
        throw DbalException(std::string("Error executing query: ").append(e.what()));
    }
}

void Query::executeAsync(Connection const& con)
{
    execValidation();

    try {
        mode = ExecMode::EXEC_ASYNC;
        future = cass_session_execute(con.getSessionPtr(), statement);
    } catch (std::exception const& e) {
        throw DbalException(std::string("Error executing query: ").append(e.what()));
    }
}

void Query::wait()
{
    if (mode != ExecMode::EXEC_ASYNC) {
        throw bes::dbal::DbalException("Query is not running and cannot wait");
    }

    try {
        cass_future_wait(future);
        mode = ExecMode::COMPLETE;

        CassError rc = cass_future_error_code(future);
        if (rc != CASS_OK) {
            throw DbalException(Utility::getFutureErrMsg(future));
        }

    } catch (std::exception const& e) {
        throw DbalException(std::string("Error executing query: ").append(e.what()));
    }
}

template <class T>
void Query::bind(T v)
{
    throw bes::dbal::DbalException("Unsupported data type for CQL bind");
}

template <>
void Query::bind<std::string>(std::string v)
{
    str_cache.push_back(std::move(v));
    cass_statement_bind_string(statement, q_pos, str_cache.back().c_str());
    ++q_pos;
}

template <>
void Query::bind<bool>(bool v)
{
    cass_statement_bind_bool(statement, q_pos, v ? cass_true : cass_false);
    ++q_pos;
}

template <>
void Query::bind<float>(float v)
{
    cass_statement_bind_float(statement, q_pos, v);
    ++q_pos;
}

template <>
void Query::bind<double>(double v)
{
    cass_statement_bind_double(statement, q_pos, v);
    ++q_pos;
}

template <>
void Query::bind<int32_t>(int32_t v)
{
    cass_statement_bind_int32(statement, q_pos, v);
    ++q_pos;
}

template <>
void Query::bind<int64_t>(int64_t v)
{
    cass_statement_bind_int64(statement, q_pos, v);
    ++q_pos;
}

void Query::execValidation() const
{
    if (mode != ExecMode::PENDING) {
        throw bes::dbal::DbalException("Query execution already begun");
    }

    if (q_pos != expected_params) {
        throw bes::dbal::DbalException("CQL parameter count mismatch: expected " + std::to_string(expected_params) +
                                       ", have " + std::to_string(q_pos));
    }
}

ResultT Query::getResult()
{
    auto cass_result = const_cast<CassResult*>(cass_future_get_result(future));
    return ResultT(std::shared_ptr<CassResult>(cass_result,
                                               [](CassResult* item) {
                                                   cass_result_free(item);
                                               }),
                   Utility::getColumnsForResult(cass_result));
}

ResultT Query::getResult(Connection const& con)
{
    executeAsync(con);
    return getResult();
}

}  // namespace bes::dbal::wide::cassandra

#endif
