#pragma once

#include <string>
#include <vector>

#include "../../exception.h"
#include "../cell.tcc"
#include "cassandra.h"
#include "connection.h"
#include "utility.h"

namespace bes::dbal::wide {
class Cassandra;
}  // namespace bes::dbal::wide

namespace bes::dbal::wide::cassandra {

class Result;

/**
 * RAII query container.
 *
 * Not thread safe.
 */
class Query
{
   public:
    explicit Query(std::string cql, size_t num_params = 0);

    void bind();  // binds a null value
    void bind(std::string v);
    void bind(Boolean v);
    void bind(Int32 v);
    void bind(Int64 v);
    void bind(Float32 v);
    void bind(Float64 v);

   private:
    enum class ExecMode
    {
        PENDING,
        EXEC_ASYNC,
        COMPLETE,
        FAILED,
    };

    /**
     * Throws a DbalException if the query is not in the correct state to execute.
     */
    void execValidation() const;

    /**
     * Begins a query execution and returns immediately.
     */
    void execute(Connection const& con);

    /**
     * Waits for query completion and results the result.
     */
    CassResult* getResult();

    std::string cql;
    std::vector<std::string> str_cache;
    std::shared_ptr<CassFuture> future;
    std::shared_ptr<CassStatement> statement;
    size_t q_pos = 0;
    size_t expected_params = 0;
    ExecMode mode = ExecMode::PENDING;

    friend class ::bes::dbal::wide::Cassandra;
    friend class ::bes::dbal::wide::cassandra::Result;
};

// --------------------------------- //

inline Query::Query(std::string cql, size_t num_params) : cql(std::move(cql)), expected_params(num_params)
{
    statement = std::shared_ptr<CassStatement>(cass_statement_new(this->cql.c_str(), num_params), [](CassStatement* s) {
        cass_statement_free(s);
    });
}

inline void Query::execute(Connection const& con)
{
    execValidation();

    try {
        mode = ExecMode::EXEC_ASYNC;
        future =
            std::shared_ptr<CassFuture>(cass_session_execute(con.getSessionPtr(), statement.get()), [](CassFuture* f) {
                cass_future_free(f);
            });
    } catch (std::exception const& e) {
        mode = ExecMode::FAILED;
        throw DbalException(std::string("Cassandra: ").append(e.what()));
    }
}

inline CassResult* Query::getResult()
{
    if (mode != ExecMode::EXEC_ASYNC) {
        throw DbalException("Query not in execution mode, call execute() first.");
    }

    mode = ExecMode::COMPLETE;
    cass_future_wait(future.get());

    CassError rc = cass_future_error_code(future.get());
    if (rc != CASS_OK) {
        mode = ExecMode::FAILED;
        throw DbalException("Cassandra: " + Utility::getFutureErrMsg(future.get()));
    }

    return const_cast<CassResult*>(cass_future_get_result(future.get()));
}

inline void Query::bind()
{
    cass_statement_bind_null(statement.get(), q_pos);
    ++q_pos;
}

inline void Query::bind(std::string v)
{
    str_cache.push_back(std::move(v));
    cass_statement_bind_string(statement.get(), q_pos, str_cache.back().c_str());
    ++q_pos;
}

inline void Query::bind(Boolean v)
{
    cass_statement_bind_bool(statement.get(), q_pos, v ? cass_true : cass_false);
    ++q_pos;
}

inline void Query::bind(Float32 v)
{
    cass_statement_bind_float(statement.get(), q_pos, v);
    ++q_pos;
}

inline void Query::bind(Float64 v)
{
    cass_statement_bind_double(statement.get(), q_pos, v);
    ++q_pos;
}

inline void Query::bind(Int32 v)
{
    cass_statement_bind_int32(statement.get(), q_pos, v);
    ++q_pos;
}

inline void Query::bind(Int64 v)
{
    cass_statement_bind_int64(statement.get(), q_pos, v);
    ++q_pos;
}

inline void Query::execValidation() const
{
    if (mode != ExecMode::PENDING) {
        throw bes::dbal::DbalException("Query execution already begun");
    }

    if (q_pos != expected_params) {
        throw bes::dbal::DbalException("CQL parameter count mismatch: expected " + std::to_string(expected_params) +
                                       ", have " + std::to_string(q_pos));
    }
}

}  // namespace bes::dbal::wide::cassandra
