#pragma once

#include "Protos/Controller/Configuration.pb.h"
#include "Protos/Controller/Configuration.grpc.pb.h"

namespace CSOL_Utilities
{
class ConfigurationService : public Protos::Controller::Configuration::Service
{
public:
    virtual ::grpc::Status Retrieve(::grpc::ServerContext* context, const Protos::Controller::RetrieveRequest* request, Protos::Controller::RetrieveResponse* response) override;
    virtual ::grpc::Status Update(::grpc::ServerContext* context, const Protos::Controller::UpdateRequest* request, Protos::Controller::UpdateResponse* response) override;
};
}