#include <QGuiApplication>
#include <QIcon>
#include <QQmlApplicationEngine>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine(&app);

    app.setWindowIcon(QIcon(":/resources/icon.png"));

    engine.loadFromModule("GUI", "Main");

    return app.exec();
}
