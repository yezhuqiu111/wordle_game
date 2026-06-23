#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtQml/qqml.h>

#include "parallelengine.h"
#include "wordlegame.h"

int main(int argc, char *argv[])
{
    qputenv("NO_COLOR", "1");

    ParallelEngine::initialize(&argc, &argv);

#ifdef WORDLE_USE_MPI
    if (ParallelEngine::mpiEnabled() && ParallelEngine::mpiRank() != 0) {
        const QStringList words = WordleGame::loadWordList();
        ParallelEngine::runDictionaryWorker(words);
        ParallelEngine::finalize();
        return 0;
    }
#endif

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

    const int code = QGuiApplication::exec();

    ParallelEngine::shutdownMpiWorkers();
    ParallelEngine::finalize();
    return code;
}
