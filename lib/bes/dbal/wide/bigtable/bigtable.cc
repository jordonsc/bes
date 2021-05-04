#include "bigtable.h"

#include <utility>

#include "google/cloud/bigtable/table.h"
#include "result.h"

namespace cbt = google::cloud::bigtable;
using google::cloud::Status;
using google::cloud::StatusOr;

using namespace bes::dbal::wide;

BigTable::BigTable(Context c) : WideColumnDb(std::move(c)), credentials(getContext()) {}

/**
 * Currently, GC rules aren't part of any common functionality. If you need tight control of this, provision tables
 * outside of the DBAL.
 *
 * When creating a table here, we'll use a standard rule of MaxNumVersions(1).
 */
ResultFuture BigTable::createTable(std::string const& table_name, Schema const& schema, bool if_not_exists) const
{
    cbt::GcRule gc_std = cbt::GcRule::MaxNumVersions(1);
    std::map<std::string, cbt::GcRule> col_families;

    col_families[schema.primary_key.ns] = gc_std;

    for (auto const& field : schema.fields) {
        if (col_families.find(field.ns) == col_families.end()) {
            col_families[field.ns] = gc_std;
        }
    }

    StatusOr<google::bigtable::admin::v2::Table> new_schema =
        getTableAdmin().CreateTable(table_name, cbt::TableConfig(std::move(col_families), {}));

    // TODO: change this to a "SuccessFuture" as it is never expected to return more than a success/fail
    return ResultFuture(std::make_shared<bes::dbal::wide::bigtable::Result>());
}

ResultFuture BigTable::dropTable(std::string const& table_name, bool if_exists) const
{
    // Delete a table
    Status status = getTableAdmin().DeleteTable(table_name);

    return ResultFuture(std::make_shared<bes::dbal::wide::bigtable::Result>());
}

ResultFuture BigTable::insert(std::string const& table_name, ValueList values) const
{
    // BigTable cannot do a generic insert, it requires a primary key at all times
    // TODO: remove the "insert" function, replace both insert and update with an "apply" call
    return ResultFuture(std::make_shared<bes::dbal::wide::bigtable::Result>());
}

ResultFuture BigTable::update(std::string const& table_name, Value const& key, ValueList values) const
{
    cbt::SingleRowMutation mutation(getKeyFromValue(key));

    auto timestamp =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());

    for (auto& v : values) {
        switch (v.getDatatype()) {
            default:
                throw DbalException("Unknown datatype for BigTable update");
            case Datatype::Null:
                throw DbalException("TODO: not sure how to do a null value here..");
            case Datatype::Text:
                mutation.emplace_back(
                    cbt::SetCell(v.getNs(), v.getQualifier(), timestamp, std::any_cast<Text&&>(v.consumeValue())));
                break;
            case Datatype::Boolean:
                mutation.emplace_back(
                    cbt::SetCell(v.getNs(), v.getQualifier(), timestamp, std::any_cast<Boolean>(v.getValue())));
                break;
            case Datatype::Int32:
                mutation.emplace_back(
                    cbt::SetCell(v.getNs(), v.getQualifier(), timestamp, std::any_cast<Int32>(v.getValue())));
                break;
            case Datatype::Int64:
                mutation.emplace_back(
                    cbt::SetCell(v.getNs(), v.getQualifier(), timestamp, std::any_cast<Int64>(v.getValue())));
                break;
            case Datatype::Float32:
                mutation.emplace_back(
                    cbt::SetCell(v.getNs(), v.getQualifier(), timestamp, std::any_cast<Float32>(v.getValue())));
                break;
            case Datatype::Float64:
                mutation.emplace_back(
                    cbt::SetCell(v.getNs(), v.getQualifier(), timestamp, std::any_cast<Float64>(v.getValue())));
                break;
        }
    }

    auto status = getTable(table_name).Apply(std::move(mutation));

    if (!status.ok()) {
        throw std::runtime_error(status.message());
    }

    return ResultFuture(std::make_shared<bes::dbal::wide::bigtable::Result>());
}

ResultFuture BigTable::retrieve(std::string const& table_name, Value const& key) const
{
    using google::cloud::bigtable::Filter;

    cbt::RowSet rs(getKeyFromValue(key));

    return ResultFuture(std::make_shared<bes::dbal::wide::bigtable::Result>(getTable(table_name).ReadRows(std::move(rs), Filter::PassAllFilter())));
}

ResultFuture BigTable::retrieve(std::string const& table_name, Value const& key, FieldList fields) const
{
    // auto filter = Filter::Chain(google::cloud::bigtable::Filter::ColumnName());
    return ResultFuture(std::make_shared<bes::dbal::wide::bigtable::Result>());
}

ResultFuture BigTable::remove(std::string const& table_name, Value const& key) const
{
    return ResultFuture(std::make_shared<bes::dbal::wide::bigtable::Result>());
}

ResultFuture BigTable::truncate(std::string const& table_name) const
{
    return ResultFuture(std::make_shared<bes::dbal::wide::bigtable::Result>());
}

google::cloud::bigtable::Table BigTable::getTable(std::string const& tbl) const
{
    return cbt::Table(
        cbt::CreateDefaultDataClient(credentials.project_id, credentials.instance_id, cbt::ClientOptions()),
        tbl);
}

google::cloud::bigtable::TableAdmin BigTable::getTableAdmin() const
{
    return cbt::TableAdmin(
        cbt::CreateDefaultAdminClient(credentials.project_id, cbt::ClientOptions()),
        credentials.instance_id);
}

/**
 * Cloud BigTable only supports string keys. We'll convert supported datatypes to string form.
 */
std::string BigTable::getKeyFromValue(Value v)
{
    switch (v.getDatatype()) {
        default:
        case Datatype::Boolean:
        case Datatype::Null:
            throw DbalException("Unsupported datatype for primary key");
        case Datatype::Text:
            return std::any_cast<Text&&>(v.consumeValue());
        case Datatype::Int32:
            return std::to_string(std::any_cast<Int32>(v.getValue()));
        case Datatype::Int64:
            return std::to_string(std::any_cast<Int64>(v.getValue()));
        case Datatype::Float32:
            return std::to_string(std::any_cast<Float32>(v.getValue()));
        case Datatype::Float64:
            return std::to_string(std::any_cast<Float64>(v.getValue()));
    }
}
