#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickItem>
#include "../net/connection.hpp"

int main(int argc, char *argv[])
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    qmlRegisterType<CPConnection>("CutiePoi.Connection", 1, 0, "CPConnection");
    qmlRegisterType<CPSharedObject>("CutiePoi.SharedObject", 1, 0, "CPSharedObject");
    qRegisterMetaType<CPSharedObject::Character>();
    qRegisterMetaType<CPSharedObject::MessagePosition>();
    qRegisterMetaType<CPSharedObject::Direction>();

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/ChatMain.qml")));
    // QQuickItem *rootObj = view.rootObject();
    // QQuickItem *swipeView = rootObj->findChild<QQuickItem*>("swipeView");
    //QMetaObject::invokeMethod();

    return app.exec();
}
