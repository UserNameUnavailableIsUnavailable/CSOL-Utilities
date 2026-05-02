#include "MainWindow.hpp"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("CSOL-Utilities NativePanel");
    app.setApplicationDisplayName("CSOL-Utilities Control Panel");

    MainWindow window;
    window.show();

    return app.exec();
}
