#ifndef BES_RPC_EXCEPTION_H
#define BES_RPC_EXCEPTION_H

#include <bes/core.h>
#include <bes/log.h>
#include <grpcpp/grpcpp.h>

#include <utility>

namespace bes::rpc {

class ServiceException : public bes::BesException
{
    using BesException::BesException;
};

class RpcException : public bes::BesException
{
    using BesException::BesException;
};

class NotFoundException : public RpcException
{
    using RpcException::RpcException;
};

class RpcCallException : public RpcException
{
   public:
    RpcCallException(grpc::Status rpc_stat) : RpcException(rpc_stat.error_message()), rpc_status(std::move(rpc_stat)) {}
    RpcCallException(grpc::Status rpc_stat, std::string const& msg) : RpcException(msg), rpc_status(std::move(rpc_stat))
    {}

    [[nodiscard]] constexpr grpc::Status const& GetRpcStatus() const
    {
        return rpc_status;
    }

    /**
     * Logs an RPC failure if the status is not OK, otherwise does nothing.
     */
    void LogRpcReply(std::string const& prefix)
    {
        if (!rpc_status.ok()) {
            std::stringstream details;
            if (rpc_status.error_message().length() || rpc_status.error_details().length()) {
                details << " (";
                if (rpc_status.error_message().length()) {
                    details << rpc_status.error_message();
                    if (rpc_status.error_details().length()) {
                        details << "; ";
                    }
                }
                if (rpc_status.error_details().length()) {
                    details << rpc_status.error_details();
                }
                details << ")";
            }

            switch (rpc_status.error_code()) {
                case grpc::OK:
                    BES_LOG(ERROR) << "[" << prefix << "] RPC errored, but OK code" << details.str();
                case grpc::CANCELLED:
                    BES_LOG(ERROR) << "[" << prefix << "] RPC cancelled" << details.str();
                    break;
                case grpc::UNKNOWN:
                case grpc::INTERNAL:
                default:
                    BES_LOG(ERROR) << "[" << prefix << "] RPC unknown error" << details.str();
                    break;
                case grpc::INVALID_ARGUMENT:
                    BES_LOG(ERROR) << "[" << prefix << "] RPC invalid argument" << details.str();
                case grpc::DEADLINE_EXCEEDED:
                    BES_LOG(ERROR) << "[" << prefix << "] RPC deadline exceeded" << details.str();
                case grpc::NOT_FOUND:
                    BES_LOG(ERROR) << "[" << prefix << "] RPC not found" << details.str();
                case grpc::ALREADY_EXISTS:
                    BES_LOG(ERROR) << "[" << prefix << "] RPC already exists" << details.str();
                case grpc::PERMISSION_DENIED:
                    BES_LOG(ERROR) << "[" << prefix << "] RPC permission denied" << details.str();
                case grpc::UNAUTHENTICATED:
                    BES_LOG(ERROR) << "[" << prefix << "] RPC unauthenticated" << details.str();
                case grpc::RESOURCE_EXHAUSTED:
                    BES_LOG(ERROR) << "[" << prefix << "] RPC resource exhausted" << details.str();
                case grpc::FAILED_PRECONDITION:
                    BES_LOG(ERROR) << "[" << prefix << "] RPC failed precondition" << details.str();
                case grpc::ABORTED:
                    BES_LOG(ERROR) << "[" << prefix << "] RPC aborted" << details.str();
                case grpc::OUT_OF_RANGE:
                    BES_LOG(ERROR) << "[" << prefix << "] RPC out of range" << details.str();
                case grpc::UNIMPLEMENTED:
                    BES_LOG(ERROR) << "[" << prefix << "] RPC unimplemented" << details.str();
                case grpc::UNAVAILABLE:
                    BES_LOG(ERROR) << "[" << prefix << "] RPC unavailable (retry)" << details.str();
                case grpc::DATA_LOSS:
                    BES_LOG(ERROR) << "[" << prefix << "] RPC unrecoverable data loss or corruption" << details.str();
            }
        }
    }

   protected:
    grpc::Status const rpc_status;
};

}  // namespace bes::rpc

#endif
