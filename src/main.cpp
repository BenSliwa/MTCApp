#include "appcontroller.h"

#include <QByteArray>

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    AppController *controller = AppController::getInstance();
    controller->init();

    return app.exec();
}
