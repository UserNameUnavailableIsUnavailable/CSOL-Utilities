#pragma once

#include <QObject>
#include <QString>
#include <memory>
#include <utility>

// Forward-declare gRPC types so consumers do not need to pull in gRPC headers.
namespace controller
{
class ControllerService;
}
namespace grpc
{
class Channel;
}

/// Thin wrapper around the gRPC ControllerService stub.
/// All methods are synchronous with a configurable deadline; call them from a
/// worker thread or Qt::QueuedConnection to avoid blocking the UI thread.
class Communication : public QObject
{
    Q_OBJECT

  public:
    explicit Communication(QObject *parent = nullptr);
    ~Communication();

    /// Connect to the Controller gRPC server at \a serverAddress.
    /// Returns true on success (the channel is created; actual connectivity is
    /// verified on the first RPC call).
    bool Connect(const QString &serverAddress);

    /// Tear down the current connection (if any).
    void Disconnect();

    bool IsConnected() const;

    // --- RPC wrappers -------------------------------------------------------
    // Each returns {success, message} matching the proto response fields.

    std::pair<bool, QString> ListModes();
    std::pair<bool, QString> SwitchMode(const QString &mode);
    std::pair<bool, QString> ConfigureOptions(const QString &option, const QString &value);
    std::pair<bool, QString> ConfigureLocale(const QString &locale);
    std::pair<bool, QString> ConfigureHotKey(const QString &hotkey, const QString &mode);

  signals:
    void connected();
    void disconnected();

  private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};