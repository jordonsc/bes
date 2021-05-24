#include "bigtable.h"

#include <utility>

#include "cbt_result.h"
#include "google/cloud/bigtable/table.h"

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
SuccessFuture BigTable::createTable(std::string const& table_name, Schema const& schema)
{
    /*
    std::map<std::string, cbt::GcRule> col_families;

    col_families[schema.primary_key.ns] = cbt::GcRule::MaxNumVersions(1);

    for (auto const& field : schema.fields) {
        if (col_families.find(field.ns) == col_families.end()) {
            col_families[field.ns] = cbt::GcRule::MaxNumVersions(1);
        }
    }

    // TODO: requires some sort of parallelism - not a real future.
    auto result = getTableAdmin().CreateTable(table_name, cbt::TableConfig(std::move(col_families), {}));

    if (!result.ok()) {
        throw bes::dbal::DbalException(result.status().message());
    }

    return SuccessFuture(result.ok());
     */
    return SuccessFuture(false);
}

SuccessFuture BigTable::dropTable(std::string const& table_name)
{
    // Delete a table
    auto status = getTableAdmin().DeleteTable(table_name);

    if (!status.ok()) {
        throw bes::dbal::DbalException(status.message());
    }

    return SuccessFuture(status.ok());
}

SuccessFuture BigTable::apply(std::string const& table_name, Value const& key, ValueList values)
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
                // TODO: we're using a blank string for column family - think of a better approach (split string?)
                mutation.emplace_back(
                    cbt::SetCell("", v.getQualifier(), timestamp, std::any_cast<Text&&>(v.consumeValue())));
                break;
            case Datatype::Boolean:
                mutation.emplace_back(
                    cbt::SetCell("", v.getQualifier(), timestamp, std::any_cast<Boolean>(v.getValue()) ? "1" : "0"));
                break;
            case Datatype::Int32: {
                auto i32 = std::any_cast<Int32>(v.getValue());
                mutation.emplace_back(cbt::SetCell(
                    "",
                    v.getQualifier(),
                    timestamp,
                    std::string(reinterpret_cast<char*>(&i32), sizeof(Int32))));
            } break;
            case Datatype::Int64: {
                auto i64 = std::any_cast<Int64>(v.getValue());
                mutation.emplace_back(cbt::SetCell(
                    "",
                    v.getQualifier(),
                    timestamp,
                    std::string(reinterpret_cast<char*>(&i64), sizeof(Int64))));
            } break;
            case Datatype::Float32: {
                auto f32 = std::any_cast<Float32>(v.getValue());
                mutation.emplace_back(cbt::SetCell(
                    "",
                    v.getQualifier(),
                    timestamp,
                    std::string(reinterpret_cast<char*>(&f32), sizeof(Float32))));
            } break;
            case Datatype::Float64: {
                auto f64 = std::any_cast<Float64>(v.getValue());
                mutation.emplace_back(cbt::SetCell(
                    "",
                    v.getQualifier(),
                    timestamp,
                    std::string(reinterpret_cast<char*>(&f64), sizeof(Float64))));
            } break;
        }
    }

    auto status = getTable(table_name).Apply(std::move(mutation));
    if (!status.ok()) {
        throw bes::dbal::DbalException(status.message());
    }

    return SuccessFuture(status.ok());
}

ResultFuture BigTable::retrieve(std::string const& table_name, Value const& key)
{
    using google::cloud::bigtable::Filter;

    cbt::RowSet rs(getKeyFromValue(key));

    return ResultFuture(std::make_shared<bes::dbal::wide::bigtable::CbtResult>(
        getTable(table_name).ReadRows(std::move(rs), Filter::PassAllFilter())));
}

ResultFuture BigTable::retrieve(std::string const& table_name, Value const& key, FieldList fields)
{
    // auto filter = Filter::Chain(google::cloud::bigtable::Filter::ColumnName());
    return retrieve(table_name, key);
}

SuccessFuture BigTable::remove(std::string const& table_name, Value const& key)
{
    return SuccessFuture(false);
}

SuccessFuture BigTable::truncate(std::string const& table_name)
{
    return SuccessFuture(false);
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
