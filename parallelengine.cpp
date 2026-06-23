#include "parallelengine.h"

#include <QDate>
#include <QVector>

namespace {

QString wordFromIndex(const QStringList &words, quint32 seed)
{
    if (words.isEmpty())
        return QStringLiteral("APPLE");
    const int index = int(seed % quint32(words.size()));
    return words.at(index);
}

} // namespace

QString ParallelEngine::dailyWord(const QStringList &words)
{
    const QDate today = QDate::currentDate();
    const QString key = today.toString(QStringLiteral("yyyyMMdd"));
    const quint32 seed = qHash(key);
    return wordFromIndex(words, seed);
}

QString ParallelEngine::broadcastDailyWord(const QStringList &words)
{
    return dailyWord(words);
}

bool ParallelEngine::containsWord(const QStringList &words, const QString &guess)
{
    return words.contains(guess);
}

QStringList ParallelEngine::evaluateGuess(const QString &guess, const QString &target)
{
    QStringList result(5, QStringLiteral("absent"));
    QVector<bool> used(5, false);

    for (int i = 0; i < 5; ++i) {
        if (guess.at(i) == target.at(i)) {
            result[i] = QStringLiteral("correct");
            used[i] = true;
        }
    }

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

    return result;
}

QString ParallelEngine::modeDescription(const QString &mode)
{
    if (mode == QStringLiteral("daily"))
        return QStringLiteral("每日猜词：按日期生成全局统一答案");
    return QStringLiteral("经典模式：单机随机单词");
}
