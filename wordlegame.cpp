#include "wordlegame.h"

#include "parallelengine.h"

#include <QDate>
#include <QFile>
#include <QIODevice>
#include <QRandomGenerator>

WordleGame::WordleGame(QObject *parent)
    : QObject(parent)
{
    loadWords();
    newGame();
}

QStringList WordleGame::loadWordList()
{
    QStringList words;
    QFile file(QStringLiteral(":/resources/words.txt"));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return words;

    while (!file.atEnd()) {
        const QString word = QString::fromUtf8(file.readLine()).trimmed().toUpper();
        if (word.length() == kWordLen)
            words.append(word);
    }
    return words;
}

QVariantList WordleGame::board() const
{
    QVariantList rows;
    for (const auto &row : m_board) {
        QVariantList cols;
        for (const auto &cell : row)
            cols.append(cell);
        rows.append(cols);
    }
    return rows;
}

QVariantMap WordleGame::keyboardStates() const
{
    QVariantMap map;
    for (auto it = m_keyboardStates.constBegin(); it != m_keyboardStates.constEnd(); ++it)
        map.insert(it.key(), it.value());
    return map;
}

void WordleGame::loadWords()
{
    m_allWords = loadWordList();
}

QStringList WordleGame::availableModes() const
{
    return {
        QStringLiteral("normal"),
        QStringLiteral("daily"),
    };
}

QString WordleGame::modeDisplayName(const QString &mode) const
{
    if (mode == QStringLiteral("daily"))
        return QStringLiteral("每日猜词");
    return QStringLiteral("经典");
}

QString WordleGame::modeDescription() const
{
    return ParallelEngine::modeDescription(m_gameMode);
}

QString WordleGame::dailyDate() const
{
    return QDate::currentDate().toString(QStringLiteral("yyyy-MM-dd"));
}

void WordleGame::setGameMode(const QString &mode)
{
    QString normalized = mode;
    if (!availableModes().contains(normalized))
        normalized = QStringLiteral("normal");
    if (m_gameMode == normalized)
        return;

    m_gameMode = normalized;
    emit gameModeChanged();
    newGame();
}

void WordleGame::pickTargetWord()
{
    if (m_allWords.isEmpty()) {
        m_targetWord = QStringLiteral("APPLE");
        return;
    }

    if (m_gameMode == QStringLiteral("daily")) {
        m_targetWord = ParallelEngine::broadcastDailyWord(m_allWords);
    } else {
        const int index = QRandomGenerator::global()->bounded(m_allWords.size());
        m_targetWord = m_allWords.at(index);
    }
}

QVariantMap WordleGame::makeCell(const QString &letter, const QString &state, bool revealed) const
{
    return QVariantMap{
        {QStringLiteral("letter"), letter},
        {QStringLiteral("state"), state},
        {QStringLiteral("revealed"), revealed},
    };
}

void WordleGame::resetBoard()
{
    m_board.clear();
    m_board.resize(kMaxRows);
    for (int row = 0; row < kMaxRows; ++row) {
        m_board[row].resize(kWordLen);
        for (int col = 0; col < kWordLen; ++col)
            m_board[row][col] = makeCell(QString(), QStringLiteral("empty"), false);
    }
}

void WordleGame::newGame()
{
    pickTargetWord();
    m_currentRow = 0;
    m_currentCol = 0;
    m_gameStatus = QStringLiteral("playing");
    m_message.clear();
    m_keyboardStates.clear();
    m_submittedGuesses.clear();
    resetBoard();

    notifyBoardChanged();
    emit keyboardStatesChanged();
    emit currentRowChanged();
    emit gameStatusChanged();
    emit messageChanged();
}

void WordleGame::addLetter(const QString &letter)
{
    if (m_gameStatus != QStringLiteral("playing"))
        return;
    if (m_currentCol >= kWordLen)
        return;

    const QString ch = letter.left(1).toUpper();
    if (ch.length() != 1 || ch.at(0) < QLatin1Char('A') || ch.at(0) > QLatin1Char('Z'))
        return;

    m_board[m_currentRow][m_currentCol] = makeCell(ch, QStringLiteral("filled"), false);
    ++m_currentCol;
    setMessage(QString());
    notifyBoardChanged();
}

void WordleGame::removeLetter()
{
    if (m_gameStatus != QStringLiteral("playing"))
        return;
    if (m_currentCol <= 0)
        return;

    --m_currentCol;
    m_board[m_currentRow][m_currentCol] = makeCell(QString(), QStringLiteral("empty"), false);
    setMessage(QString());
    notifyBoardChanged();
}

QStringList WordleGame::evaluateGuess(const QString &guess)
{
    return ParallelEngine::evaluateGuess(guess, m_targetWord);
}

int WordleGame::stateRank(const QString &state) const
{
    if (state == QStringLiteral("correct"))
        return 3;
    if (state == QStringLiteral("present"))
        return 2;
    if (state == QStringLiteral("absent"))
        return 1;
    return 0;
}

void WordleGame::updateKeyboardLetter(const QChar &letter, const QString &state)
{
    const QString key = QString(letter).toUpper();
    const QString existing = m_keyboardStates.value(key);
    if (stateRank(state) > stateRank(existing))
        m_keyboardStates[key] = state;
}

bool WordleGame::submitGuess()
{
    if (m_gameStatus != QStringLiteral("playing"))
        return false;
    if (m_currentCol < kWordLen) {
        setMessage(QStringLiteral("字母不够"));
        emit invalidGuess();
        return false;
    }

    QString guess;
    for (int col = 0; col < kWordLen; ++col)
        guess += m_board[m_currentRow][col].value(QStringLiteral("letter")).toString();

    if (!ParallelEngine::containsWord(m_allWords, guess)) {
        setMessage(QStringLiteral("不在词库中"));
        emit invalidGuess();
        return false;
    }

    if (m_submittedGuesses.contains(guess)) {
        setMessage(QStringLiteral("已经猜过这个词"));
        emit invalidGuess();
        return false;
    }

    m_submittedGuesses.append(guess);

    const QStringList evaluation = evaluateGuess(guess);
    for (int col = 0; col < kWordLen; ++col) {
        const QString state = evaluation.at(col);
        const QChar letter = guess.at(col);
        m_board[m_currentRow][col] = makeCell(QString(letter), state, false);
        updateKeyboardLetter(letter, state);
    }

    const int submittedRow = m_currentRow;
    const bool won = guess == m_targetWord;

    if (won) {
        m_gameStatus = QStringLiteral("won");
        setMessage(QString());
    } else if (m_currentRow >= kMaxRows - 1) {
        m_gameStatus = QStringLiteral("lost");
        setMessage(QString());
    } else {
        m_currentRow++;
        m_currentCol = 0;
        setMessage(QString());
    }

    notifyBoardChanged();
    emit keyboardStatesChanged();
    emit currentRowChanged();
    emit gameStatusChanged();
    emit rowSubmitted(submittedRow);
    return true;
}

void WordleGame::setMessage(const QString &message)
{
    if (m_message == message)
        return;
    m_message = message;
    emit messageChanged();
}

void WordleGame::notifyBoardChanged()
{
    ++m_boardRevision;
    emit boardChanged();
}

QString WordleGame::cellLetter(int row, int col) const
{
    if (row < 0 || row >= kMaxRows || col < 0 || col >= kWordLen)
        return QString();
    return m_board[row][col].value(QStringLiteral("letter")).toString();
}

QString WordleGame::cellState(int row, int col) const
{
    if (row < 0 || row >= kMaxRows || col < 0 || col >= kWordLen)
        return QStringLiteral("empty");
    return m_board[row][col].value(QStringLiteral("state")).toString();
}

QString WordleGame::currentRowText() const
{
    QString text;
    for (int col = 0; col < kWordLen; ++col) {
        const QString letter = m_board[m_currentRow][col].value(QStringLiteral("letter")).toString();
        text += letter.isEmpty() ? QStringLiteral("_") : letter;
    }
    return text;
}
