#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setOrganizationName(QStringLiteral("HymnBook"));
    app.setApplicationName(QStringLiteral("HymnBookApp"));
    app.setApplicationVersion(QStringLiteral("1.0.0"));

    MainWindow window;
    window.show();

    return app.exec();
}
