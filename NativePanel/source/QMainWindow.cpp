#include "MainWindow.hpp"
#include "Communication.hpp"

#include <QCheckBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMap>
#include <QPushButton>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , comm_(new Communication(this))
{
    setWindowTitle("CSOL-Utilities Control Panel");
    setMinimumWidth(420);

    connect(comm_, &Communication::connected,    this, &MainWindow::onCommunicationConnected);
    connect(comm_, &Communication::disconnected, this, &MainWindow::onCommunicationDisconnected);

    setupUi();
    setControlsEnabled(false);
}

MainWindow::~MainWindow() = default;

// ---------------------------------------------------------------------------
void MainWindow::setupUi()
{
    auto *central = new QWidget(this);
    setCentralWidget(central);
    auto *root = new QVBoxLayout(central);
    root->setSpacing(8);
    root->setContentsMargins(10, 10, 10, 10);

    // --- Connection ---------------------------------------------------------
    {
        auto *grp    = new QGroupBox("Connection", central);
        auto *layout = new QHBoxLayout(grp);

        addressEdit_ = new QLineEdit("localhost:50051", grp);
        addressEdit_->setPlaceholderText("host:port");

        connectBtn_ = new QPushButton("Connect", grp);
        connect(connectBtn_, &QPushButton::clicked, this, &MainWindow::onConnectClicked);

        layout->addWidget(new QLabel("Server:", grp));
        layout->addWidget(addressEdit_, 1);
        layout->addWidget(connectBtn_);
        root->addWidget(grp);
    }

    // --- Mode ---------------------------------------------------------------
    {
        auto *grp    = new QGroupBox("Mode", central);
        auto *layout = new QHBoxLayout(grp);

        const QStringList modes = {
            "None", "DefaultIdle", "ExtendedIdle",
            "BatchCombineParts", "BatchPurchaseItem", "LocateCursor"
        };
        for (const QString &m : modes)
        {
            auto *btn = new QPushButton(m, grp);
            modeButtons_[m] = btn;
            connect(btn, &QPushButton::clicked, this, [this, m]() {
                onSwitchModeClicked(m);
            });
            layout->addWidget(btn);
        }
        root->addWidget(grp);
    }

    // --- Options ------------------------------------------------------------
    {
        auto *grp    = new QGroupBox("Options", central);
        auto *layout = new QVBoxLayout(grp);

        disableFullscreenCheck_ = new QCheckBox("Disable Quick Fullscreen (Alt+Enter)", grp);
        suppressBannerCheck_    = new QCheckBox("Suppress CSO Banner", grp);

        connect(disableFullscreenCheck_, &QCheckBox::toggled, this, &MainWindow::onOptionToggled);
        connect(suppressBannerCheck_,    &QCheckBox::toggled, this, &MainWindow::onOptionToggled);

        layout->addWidget(disableFullscreenCheck_);
        layout->addWidget(suppressBannerCheck_);
        root->addWidget(grp);
    }

    // --- Locale -------------------------------------------------------------
    {
        auto *grp    = new QGroupBox("Locale", central);
        auto *layout = new QHBoxLayout(grp);

        localeEdit_     = new QLineEdit("zh-CN", grp);
        applyLocaleBtn_ = new QPushButton("Apply", grp);
        connect(applyLocaleBtn_, &QPushButton::clicked, this, &MainWindow::onApplyLocaleClicked);

        layout->addWidget(new QLabel("Locale:", grp));
        layout->addWidget(localeEdit_, 1);
        layout->addWidget(applyLocaleBtn_);
        root->addWidget(grp);
    }

    // --- Status bar ---------------------------------------------------------
    statusLabel_ = new QLabel("Disconnected");
    statusBar()->addWidget(statusLabel_, 1);
}

// ---------------------------------------------------------------------------
void MainWindow::setControlsEnabled(bool enabled)
{
    for (auto *btn : modeButtons_)
        btn->setEnabled(enabled);
    disableFullscreenCheck_->setEnabled(enabled);
    suppressBannerCheck_->setEnabled(enabled);
    localeEdit_->setEnabled(enabled);
    applyLocaleBtn_->setEnabled(enabled);
}

void MainWindow::showStatus(bool success, const QString &message)
{
    statusLabel_->setText(message);
    statusLabel_->setStyleSheet(success ? "color: green;" : "color: red;");
}

// ---------------------------------------------------------------------------
// Slots
// ---------------------------------------------------------------------------
void MainWindow::onConnectClicked()
{
    if (comm_->IsConnected())
    {
        comm_->Disconnect();
        connectBtn_->setText("Connect");
    }
    else
    {
        const QString addr = addressEdit_->text().trimmed();
        if (addr.isEmpty())
        {
            showStatus(false, "Please enter a server address.");
            return;
        }
        comm_->Connect(addr);
        connectBtn_->setText("Disconnect");
    }
}

void MainWindow::onSwitchModeClicked(const QString &mode)
{
    auto [ok, msg] = comm_->SwitchMode(mode);
    showStatus(ok, msg);
}

void MainWindow::onApplyLocaleClicked()
{
    const QString locale = localeEdit_->text().trimmed();
    if (locale.isEmpty())
    {
        showStatus(false, "Please enter a locale name (e.g. zh-CN).");
        return;
    }
    auto [ok, msg] = comm_->ConfigureLocale(locale);
    showStatus(ok, msg);
}

void MainWindow::onOptionToggled()
{
    auto *sender_check = qobject_cast<QCheckBox *>(sender());
    if (!sender_check)
        return;

    QString option, value;
    if (sender_check == disableFullscreenCheck_)
    {
        option = "DisableQuickFullscreen";
        value  = sender_check->isChecked() ? "true" : "false";
    }
    else if (sender_check == suppressBannerCheck_)
    {
        option = "SuppressCSOBanner";
        value  = sender_check->isChecked() ? "true" : "false";
    }
    else
    {
        return;
    }

    auto [ok, msg] = comm_->ConfigureOptions(option, value);
    showStatus(ok, msg);
}

void MainWindow::onCommunicationConnected()
{
    setControlsEnabled(true);
    showStatus(true, "Connected to " + addressEdit_->text().trimmed());
}

void MainWindow::onCommunicationDisconnected()
{
    setControlsEnabled(false);
    showStatus(false, "Disconnected");
}