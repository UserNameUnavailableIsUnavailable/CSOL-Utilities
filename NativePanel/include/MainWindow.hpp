#pragma once

#include <QMainWindow>
#include <QMap>

class Communication;
class QCheckBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QStatusBar;

class MainWindow : public QMainWindow
{
    Q_OBJECT

  public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

  private slots:
    void onConnectClicked();
    void onSwitchModeClicked(const QString &mode);
    void onApplyLocaleClicked();
    void onOptionToggled();
    void onCommunicationConnected();
    void onCommunicationDisconnected();

  private:
    void setupUi();
    void setControlsEnabled(bool enabled);
    void showStatus(bool success, const QString &message);

    Communication             *comm_;

    // Connection group
    QLineEdit  *addressEdit_;
    QPushButton *connectBtn_;

    // Mode group
    QMap<QString, QPushButton *> modeButtons_;

    // Options group
    QCheckBox *disableFullscreenCheck_;
    QCheckBox *suppressBannerCheck_;

    // Locale group
    QLineEdit  *localeEdit_;
    QPushButton *applyLocaleBtn_;

    // Status bar
    QLabel *statusLabel_;
};
