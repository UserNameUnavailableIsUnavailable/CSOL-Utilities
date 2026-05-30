#include <gtest/gtest.h>

#include "Server.hpp"
#include "Protos/Controller/Configuration.pb.h"
#include "Protos/Controller/Configuration.grpc.pb.h"

using namespace CSOL_Utilities;

class ConfigurationServiceImpl : public Protos::Controller::ConfigurationService::Service
{
public:
    virtual grpc::Status Retrieve(
        grpc::ServerContext* context,
        const Protos::Controller::RetrieveConfigurationRequest* request,
        Protos::Controller::RetrieveConfigurationResponse* response
    ) override
    {
        response->set_data("example");
        response->set_ok(true);
        return grpc::Status::OK;
    }
    virtual grpc::Status Update(
        grpc::ServerContext* context,
        const Protos::Controller::UpdateConfigurationRequest* request,
        Protos::Controller::UpdateConfigurationResponse* response
    ) override
    {
        std::cout << "data received:\n";
        response->set_ok(true);
        return grpc::Status::OK;
    }
    virtual ~ConfigurationServiceImpl() override = default;
};

class ConfigurationClientImpl
{
public:
    ConfigurationClientImpl(std::shared_ptr<grpc::Channel> channel) : stub_(Protos::Controller::ConfigurationService::NewStub(channel)) {}
    std::string Retrieve()
    {
        Protos::Controller::RetrieveConfigurationRequest req;
        Protos::Controller::RetrieveConfigurationResponse res;
        grpc::ClientContext ctx;
        grpc::Status status = stub_->Retrieve(&ctx, req, &res);
        if (status.ok()) {
            return res.data();
        } else {
            return "error";
        }
    }
private:
    std::unique_ptr<Protos::Controller::ConfigurationService::Stub> stub_;
};

TEST(ServerTest, ConfigService)
{
    std::mutex m;
    std::condition_variable cv;
    bool listening = false;
    Server server("localhost:50051");
    auto service = std::make_unique<ConfigurationServiceImpl>();
    server.Register("config", std::move(service));
    std::thread j([&server, &m, &cv, &listening] {
        server.Start([&m, &cv, &listening] {
            std::lock_guard<std::mutex> lock(m);
            listening = true;
            cv.notify_one();
        });
    });
    {
        std::unique_lock<std::mutex> lock(m);
        cv.wait(lock, [&listening] { return listening; });
    }
    grpc::ChannelArguments args;
    args.SetInt(GRPC_ARG_ENABLE_HTTP_PROXY, 0);
    auto channel = grpc::CreateCustomChannel(
        "localhost:50051",
        grpc::InsecureChannelCredentials(),
        args
    );
    ConfigurationClientImpl client(channel);
    std::cout << client.Retrieve() << std::endl;
    server.Stop();
    j.join();
}
