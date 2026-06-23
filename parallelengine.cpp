#include "parallelengine.h"

#include <QDate>
#include <QHash>
#include <QVector>

#ifdef _OPENMP
#include <omp.h>
#endif

#ifdef WORDLE_USE_MPI
#include <mpi.h>
#include <cstring>
#endif

namespace {

bool g_mpiInitialized = false;
int g_mpiRank = 0;
int g_mpiSize = 1;

enum { MpiOpExit = 0, MpiOpValidate = 1 };

QString wordFromIndex(const QStringList &words, quint32 seed)
{
    if (words.isEmpty())
        return QStringLiteral("APPLE");
    const int index = int(seed % quint32(words.size()));
    return words.at(index);
}

#ifdef WORDLE_USE_MPI
int searchLocalChunk(const QStringList &words, const QString &guess, int rank, int size)
{
    const int chunk = (words.size() + size - 1) / size;
    const int begin = rank * chunk;
    const int end = qMin(begin + chunk, words.size());
    for (int i = begin; i < end; ++i) {
        if (words.at(i) == guess)
            return 1;
    }
    return 0;
}

bool distributedLookup(const QStringList &words, const QString &guess, ParallelStats *stats)
{
    char guessBuf[6] = {0};
    if (g_mpiRank == 0) {
        const QByteArray bytes = guess.toLatin1();
        std::memcpy(guessBuf, bytes.constData(), 5);
    }
    MPI_Bcast(guessBuf, 5, MPI_CHAR, 0, MPI_COMM_WORLD);
    const QString sharedGuess = QString::fromLatin1(guessBuf, 5).toUpper();

    const int localFound = searchLocalChunk(words, sharedGuess, g_mpiRank, g_mpiSize);
    int globalFound = 0;
    MPI_Allreduce(&localFound, &globalFound, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
    if (stats) {
        stats->detail = QStringLiteral("MPI 分块查词: %1 个进程并行搜索词库").arg(g_mpiSize);
        stats->mpiRank = g_mpiRank;
        stats->mpiSize = g_mpiSize;
        stats->mpiEnabled = true;
    }
    return globalFound != 0;
}
#endif

} // namespace

void ParallelEngine::initialize(int *argc, char ***argv)
{
#ifdef WORDLE_USE_MPI
    int provided = 0;
    MPI_Init_thread(argc, argv, MPI_THREAD_MULTIPLE, &provided);
    MPI_Comm_rank(MPI_COMM_WORLD, &g_mpiRank);
    MPI_Comm_size(MPI_COMM_WORLD, &g_mpiSize);
    g_mpiInitialized = true;
#else
    Q_UNUSED(argc);
    Q_UNUSED(argv);
#endif
}

void ParallelEngine::finalize()
{
#ifdef WORDLE_USE_MPI
    if (g_mpiInitialized) {
        MPI_Finalize();
        g_mpiInitialized = false;
    }
#endif
}

bool ParallelEngine::mpiEnabled()
{
#ifdef WORDLE_USE_MPI
    return g_mpiInitialized;
#else
    return false;
#endif
}

int ParallelEngine::mpiRank()
{
    return g_mpiRank;
}

int ParallelEngine::mpiSize()
{
    return g_mpiSize;
}

QString ParallelEngine::dailyWord(const QStringList &words)
{
    const QDate today = QDate::currentDate();
    const QString key = today.toString(QStringLiteral("yyyyMMdd"));
    const quint32 seed = qHash(key);
    return wordFromIndex(words, seed);
}

QString ParallelEngine::broadcastDailyWord(const QStringList &words, ParallelStats *stats)
{
    ParallelStats localStats;
    if (!stats)
        stats = &localStats;

    stats->mpiRank = g_mpiRank;
    stats->mpiSize = g_mpiSize;
    stats->mpiEnabled = mpiEnabled();

#ifdef WORDLE_USE_MPI
    char buffer[6] = {0};
    if (g_mpiRank == 0) {
        const QString word = dailyWord(words);
        const QByteArray bytes = word.toLatin1();
        std::memcpy(buffer, bytes.constData(), 5);
    }
    MPI_Bcast(buffer, 5, MPI_CHAR, 0, MPI_COMM_WORLD);
    stats->detail = QStringLiteral("MPI_Bcast: Rank 0 广播每日单词 → 全部 %1 个进程").arg(g_mpiSize);
    return QString::fromLatin1(buffer, 5).toUpper();
#else
    const QString word = dailyWord(words);
    stats->detail = QStringLiteral("模拟 MPI_Bcast: 主进程按日期生成每日单词，全局一致");
    return word;
#endif
}

bool ParallelEngine::containsWord(const QStringList &words,
                                  const QString &guess,
                                  bool useOpenMp,
                                  bool useMpi,
                                  ParallelStats *stats)
{
    ParallelStats localStats;
    if (!stats)
        stats = &localStats;

    stats->mpiRank = g_mpiRank;
    stats->mpiSize = g_mpiSize;
    stats->mpiEnabled = mpiEnabled();
    stats->threadsUsed = 1;

    if (words.isEmpty())
        return false;

#ifdef WORDLE_USE_MPI
    if (useMpi && g_mpiInitialized && g_mpiSize > 1) {
        int op = MpiOpValidate;
        MPI_Bcast(&op, 1, MPI_INT, 0, MPI_COMM_WORLD);
        return distributedLookup(words, guess, stats);
    }
#else
    Q_UNUSED(useMpi);
#endif

#ifdef _OPENMP
    if (useOpenMp) {
        stats->threadsUsed = omp_get_max_threads();
        bool found = false;
        const int count = words.size();

        #pragma omp parallel for shared(found) schedule(static)
        for (int i = 0; i < count; ++i) {
            if (found)
                continue;
            if (words.at(i) == guess) {
                #pragma omp critical
                {
                    if (!found)
                        found = true;
                }
            }
        }

        stats->detail = QStringLiteral("OpenMP parallel for: %1 线程并行搜索词库")
                            .arg(stats->threadsUsed);
        return found;
    }
#else
    Q_UNUSED(useOpenMp);
#endif

    stats->detail = QStringLiteral("串行查词");
    return words.contains(guess);
}

QStringList ParallelEngine::evaluateGuess(const QString &guess,
                                          const QString &target,
                                          bool useOpenMp,
                                          ParallelStats *stats)
{
    ParallelStats localStats;
    if (!stats)
        stats = &localStats;

    stats->threadsUsed = 1;
    QStringList result(5, QStringLiteral("absent"));
    QVector<bool> used(5, false);

    for (int i = 0; i < 5; ++i) {
        if (guess.at(i) == target.at(i)) {
            result[i] = QStringLiteral("correct");
            used[i] = true;
        }
    }

#ifdef _OPENMP
    if (useOpenMp) {
        stats->threadsUsed = omp_get_max_threads();
        #pragma omp parallel for schedule(static)
        for (int i = 0; i < 5; ++i) {
            if (result.at(i) == QStringLiteral("correct"))
                continue;
            for (int j = 0; j < 5; ++j) {
                #pragma omp critical
                {
                    if (!used[j] && guess.at(i) == target.at(j)) {
                        result[i] = QStringLiteral("present");
                        used[j] = true;
                    }
                }
            }
        }
        stats->detail = QStringLiteral("OpenMP parallel for: %1 线程并行判定字母")
                            .arg(stats->threadsUsed);
        return result;
    }
#else
    Q_UNUSED(useOpenMp);
#endif

    for (int i = 0; i < 5; ++i) {
        if (result.at(i) == QStringLiteral("correct"))
            continue;
        for (int j = 0; j < 5; ++j) {
            if (!used[j] && guess.at(i) == target.at(j)) {
                result[i] = QStringLiteral("present");
                used[j] = true;
                break;
            }
        }
    }

    stats->detail = QStringLiteral("串行判定字母");
    return result;
}

QString ParallelEngine::modeDescription(const QString &mode)
{
    if (mode == QStringLiteral("daily"))
        return QStringLiteral("每日猜词：MPI_Bcast 广播全局统一答案");
    return QStringLiteral("经典模式：OpenMP 多线程并行查词与判定");
}

void ParallelEngine::runDictionaryWorker(const QStringList &words)
{
#ifdef WORDLE_USE_MPI
    while (true) {
        int op = 0;
        MPI_Bcast(&op, 1, MPI_INT, 0, MPI_COMM_WORLD);
        if (op == MpiOpExit)
            break;
        if (op == MpiOpValidate) {
            char guessBuf[6] = {0};
            MPI_Bcast(guessBuf, 5, MPI_CHAR, 0, MPI_COMM_WORLD);
            const QString guess = QString::fromLatin1(guessBuf, 5).toUpper();
            const int localFound = searchLocalChunk(words, guess, g_mpiRank, g_mpiSize);
            int globalFound = 0;
            MPI_Allreduce(&localFound, &globalFound, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
            Q_UNUSED(globalFound);
        }
    }
#else
    Q_UNUSED(words);
#endif
}

void ParallelEngine::shutdownMpiWorkers()
{
#ifdef WORDLE_USE_MPI
    if (g_mpiInitialized && g_mpiRank == 0 && g_mpiSize > 1) {
        int op = MpiOpExit;
        MPI_Bcast(&op, 1, MPI_INT, 0, MPI_COMM_WORLD);
    }
#endif
}
