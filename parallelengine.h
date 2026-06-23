#ifndef PARALLELENGINE_H
#define PARALLELENGINE_H

#include <QString>
#include <QStringList>

struct ParallelStats {
    int threadsUsed = 1;
    int mpiRank = 0;
    int mpiSize = 1;
    bool mpiEnabled = false;
    QString detail;
};

class ParallelEngine
{
public:
    static void initialize(int *argc = nullptr, char ***argv = nullptr);
    static void finalize();

    static bool mpiEnabled();
    static int mpiRank();
    static int mpiSize();

    static QString dailyWord(const QStringList &words);
    static QString broadcastDailyWord(const QStringList &words, ParallelStats *stats = nullptr);

    static bool containsWord(const QStringList &words,
                             const QString &guess,
                             bool useOpenMp,
                             bool useMpi,
                             ParallelStats *stats = nullptr);

    static QStringList evaluateGuess(const QString &guess,
                                     const QString &target,
                                     bool useOpenMp,
                                     ParallelStats *stats = nullptr);

    static QString modeDescription(const QString &mode);

    static void runDictionaryWorker(const QStringList &words);
    static void shutdownMpiWorkers();
};

#endif // PARALLELENGINE_H
