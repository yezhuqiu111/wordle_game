#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtQml/qqml.h>

#include "wordlegame.h"

int main(int argc, char *argv[])
{
    qputenv("NO_COLOR", "1");

    QGuiApplication app(argc, argv);

    WordleGame wordleGame;

    qmlRegisterUncreatableType<WordleGame>(
        "wordle_game",
        1,
        0,
        "WordleGame",
        QStringLiteral("Use the global wordleGame instance"));

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(QStringLiteral("wordleGame"), &wordleGame);

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    engine.loadFromModule("wordle_game", "Main");

    return QGuiApplication::exec();
}
