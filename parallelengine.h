#ifndef PARALLELENGINE_H
#define PARALLELENGINE_H

#include <QString>
#include <QStringList>

class ParallelEngine
{
public:
    static QString dailyWord(const QStringList &words);
    static QString broadcastDailyWord(const QStringList &words);

    static bool containsWord(const QStringList &words, const QString &guess);
    static QStringList evaluateGuess(const QString &guess, const QString &target);

    static QString modeDescription(const QString &mode);
};

#endif // PARALLELENGINE_H
