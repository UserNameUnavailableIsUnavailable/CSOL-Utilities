#pragma once

#include <grpcpp/grpcpp.h>

#include "NonCopyable.hpp"

namespace CSOL_Utilities
{
class Server : public NonCopyable
{
    enum class Status
    {
        kIdle,
        kLocked,
        kRunning,
    };
public:
    explicit Server(const std::string& address);
    ~Server() noexcept;

    void Start(std::function<void()> on_before_wait = nullptr);
    void Stop();
    void Register(std::string name, std::unique_ptr<grpc::Service> service);

private:
    std::atomic<Status> status_{ Status::kIdle };
    std::string address_;
    grpc::ServerBuilder server_builder_;
    std::unordered_map<std::string, std::unique_ptr<grpc::Service>> services_;
    std::unique_ptr<grpc::Server> server_;
};
}
