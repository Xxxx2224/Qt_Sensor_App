#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "authmanager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QQmlApplicationEngine engine;

    AuthManager authManager;
    engine.rootContext()->setContextProperty("Backend", &authManager);


    const QUrl url("qrc:/qt/qml/YeniSensor/ui/Main.qml");

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
                         if (!obj && url == objUrl)
                             QCoreApplication::exit(-1);
                     }, Qt::QueuedConnection);

    engine.load(url);
    return app.exec();
}
