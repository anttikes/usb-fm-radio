#include <QGuiApplication>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQuickStyle>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine(&app);

    QQuickStyle::setStyle("Basic");

    app.setWindowIcon(QIcon(":/resources/icon.png"));

    engine.loadFromModule("GUI", "Main");

    return app.exec();
}
