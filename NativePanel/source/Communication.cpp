#include <grpcpp/grpcpp.h>
#include "Controller.grpc.pb.h"
#include "Controller.pb.h"

#include "Communication.hpp"

#include <chrono>

// ---------------------------------------------------------------------------
// Impl
// ---------------------------------------------------------------------------
struct Communication::Impl
{
    std::shared_ptr<grpc::Channel>                    channel;
    std::unique_ptr<controller::ControllerService::Stub> stub;
    bool connected = false;

    static constexpr int kDeadlineMs = 5000;

    grpc::ClientContext MakeContext()
    {
        grpc::ClientContext ctx;
        ctx.set_deadline(std::chrono::system_clock::now() +
                         std::chrono::milliseconds(kDeadlineMs));
        return ctx;
    }
};

// ---------------------------------------------------------------------------
// Communication
// ---------------------------------------------------------------------------
Communication::Communication(QObject *parent)
    : QObject(parent), impl_(std::make_unique<Impl>())
{
}

Communication::~Communication() = default;

bool Communication::Connect(const QString &serverAddress)
{
    impl_->channel = grpc::CreateChannel(serverAddress.toStdString(),
                                         grpc::InsecureChannelCredentials());
    impl_->stub      = controller::ControllerService::NewStub(impl_->channel);
    impl_->connected = true;
    emit connected();
    return true;
}

void Communication::Disconnect()
{
    impl_->stub.reset();
    impl_->channel.reset();
    impl_->connected = false;
    emit disconnected();
}

bool Communication::IsConnected() const
{
    return impl_->connected;
}

// ---------------------------------------------------------------------------
// RPC helpers
// ---------------------------------------------------------------------------
std::pair<bool, QString> Communication::ListModes()
{
    if (!impl_->connected)
        return {false, "Not connected"};

    controller::SwitchModeRequest  req;
    controller::SwitchModeResponse resp;
    req.set_mode(""); // empty => list modes
    auto ctx    = impl_->MakeContext();
    auto status = impl_->stub->SwitchMode(&ctx, req, &resp);
    if (!status.ok())
        return {false, QString::fromStdString(status.error_message())};
    return {resp.success(), QString::fromStdString(resp.message())};
}

std::pair<bool, QString> Communication::SwitchMode(const QString &mode)
{
    if (!impl_->connected)
        return {false, "Not connected"};

    controller::SwitchModeRequest  req;
    controller::SwitchModeResponse resp;
    req.set_mode(mode.toStdString());
    auto ctx    = impl_->MakeContext();
    auto status = impl_->stub->SwitchMode(&ctx, req, &resp);
    if (!status.ok())
        return {false, QString::fromStdString(status.error_message())};
    return {resp.success(), QString::fromStdString(resp.message())};
}

std::pair<bool, QString> Communication::ConfigureOptions(const QString &option,
                                                          const QString &value)
{
    if (!impl_->connected)
        return {false, "Not connected"};

    controller::ConfigureOptionsRequest  req;
    controller::ConfigureOptionsResponse resp;
    req.set_option(option.toStdString());
    req.set_value(value.toStdString());
    auto ctx    = impl_->MakeContext();
    auto status = impl_->stub->ConfigureOptions(&ctx, req, &resp);
    if (!status.ok())
        return {false, QString::fromStdString(status.error_message())};
    return {resp.success(), QString::fromStdString(resp.message())};
}

std::pair<bool, QString> Communication::ConfigureLocale(const QString &locale)
{
    if (!impl_->connected)
        return {false, "Not connected"};

    controller::ConfigureLocaleRequest  req;
    controller::ConfigureLocaleResponse resp;
    req.set_locale(locale.toStdString());
    auto ctx    = impl_->MakeContext();
    auto status = impl_->stub->ConfigureLocale(&ctx, req, &resp);
    if (!status.ok())
        return {false, QString::fromStdString(status.error_message())};
    return {resp.success(), QString::fromStdString(resp.message())};
}

std::pair<bool, QString> Communication::ConfigureHotKey(const QString &hotkey,
                                                         const QString &mode)
{
    if (!impl_->connected)
        return {false, "Not connected"};

    controller::ConfigureHotKeyRequest  req;
    controller::ConfigureHotKeyResponse resp;
    req.set_hotkey(hotkey.toStdString());
    req.set_mode(mode.toStdString());
    auto ctx    = impl_->MakeContext();
    auto status = impl_->stub->ConfigureHotKey(&ctx, req, &resp);
    if (!status.ok())
        return {false, QString::fromStdString(status.error_message())};
    return {resp.success(), QString::fromStdString(resp.message())};
}