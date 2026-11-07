#include "Server.hpp"

namespace CSOL_Utilities
{
Server::Server(const std::string& address)
    : address_(address)
{}

static const char* kErrorNotIdle = "The server has already been started or is being started.";
static const char* kErrorNotRunning = "The server is not running.";
void Server::Register(std::string name, std::unique_ptr<grpc::Service> service)
{
    if (status_.load(std::memory_order_acquire) != Status::kIdle)
    {
        throw std::runtime_error(kErrorNotIdle);
    }
    services_[name] = std::move(service);
}

void Server::Start(std::function<void()> on_before_wait)
{
    auto expected = Status::kIdle;
    bool ok = status_.compare_exchange_strong(expected, Status::kLocked, std::memory_order_acq_rel);
    if (!ok)
    {
        throw std::runtime_error(kErrorNotIdle);
    }
    server_builder_.AddListeningPort(address_, grpc::InsecureServerCredentials());

    for (const auto& [name, svc] : services_)
    {
        server_builder_.RegisterService(svc.get());
    }

    server_ = server_builder_.BuildAndStart();
    if (!server_)
    {
        status_.store(Status::kIdle, std::memory_order_release);
        throw std::runtime_error("Failed to start the server.");
    }
    status_.store(Status::kRunning, std::memory_order_release);
    if (on_before_wait)
    {
        on_before_wait();
    }
    server_->Wait();
}

void Server::Stop()
{
    if (status_.load(std::memory_order_acquire) != Status::kRunning)
    {
        throw std::runtime_error(kErrorNotRunning);
    }
    server_->Shutdown();
    status_.store(Status::kIdle, std::memory_order_release);
}

Server::~Server() noexcept
{
    if (server_ && status_.load(std::memory_order_acquire) == Status::kRunning)
    {
        server_->Shutdown();
    }
}

} // CSOL_Utilities
