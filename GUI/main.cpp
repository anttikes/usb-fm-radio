#include <QGuiApplication>
#include <QQmlApplicationEngine>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine(&app);

    engine.loadFromModule("GUI", "Main");

    return app.exec();
}
