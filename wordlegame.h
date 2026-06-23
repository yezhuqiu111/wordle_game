#ifndef WORDLEGAME_H
#define WORDLEGAME_H

#include <QMap>
#include <QObject>
#include <QStringList>
#include <QVariantList>
#include <QVariantMap>
#include <QVector>

class WordleGame : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList board READ board NOTIFY boardChanged)
    Q_PROPERTY(int boardRevision READ boardRevision NOTIFY boardChanged)
    Q_PROPERTY(QVariantMap keyboardStates READ keyboardStates NOTIFY keyboardStatesChanged)
    Q_PROPERTY(int currentRow READ currentRow NOTIFY currentRowChanged)
    Q_PROPERTY(QString gameStatus READ gameStatus NOTIFY gameStatusChanged)
    Q_PROPERTY(QString message READ message NOTIFY messageChanged)
    Q_PROPERTY(QString currentRowText READ currentRowText NOTIFY boardChanged)
    Q_PROPERTY(QString answerWord READ answerWord NOTIFY gameStatusChanged)
    Q_PROPERTY(QString gameMode READ gameMode WRITE setGameMode NOTIFY gameModeChanged)
    Q_PROPERTY(QString modeDescription READ modeDescription NOTIFY gameModeChanged)
    Q_PROPERTY(QString dailyDate READ dailyDate NOTIFY gameModeChanged)
    Q_PROPERTY(QStringList availableModes READ availableModes CONSTANT)

public:
    explicit WordleGame(QObject *parent = nullptr);

    static QStringList loadWordList();

    QVariantList board() const;
    int boardRevision() const { return m_boardRevision; }
    QVariantMap keyboardStates() const;
    int currentRow() const { return m_currentRow; }
    QString gameStatus() const { return m_gameStatus; }
    QString message() const { return m_message; }
    QString currentRowText() const;
    QString answerWord() const { return m_targetWord; }
    QString gameMode() const { return m_gameMode; }
    QString modeDescription() const;
    QString dailyDate() const;
    QStringList availableModes() const;

    Q_INVOKABLE void setGameMode(const QString &mode);
    Q_INVOKABLE QString modeDisplayName(const QString &mode) const;
    Q_INVOKABLE void addLetter(const QString &letter);
    Q_INVOKABLE void removeLetter();
    Q_INVOKABLE bool submitGuess();
    Q_INVOKABLE void newGame();
    Q_INVOKABLE int wordLength() const { return kWordLen; }
    Q_INVOKABLE int maxRows() const { return kMaxRows; }
    Q_INVOKABLE QString cellLetter(int row, int col) const;
    Q_INVOKABLE QString cellState(int row, int col) const;

signals:
    void boardChanged();
    void keyboardStatesChanged();
    void currentRowChanged();
    void gameStatusChanged();
    void messageChanged();
    void gameModeChanged();
    void rowSubmitted(int row);
    void invalidGuess();

private:
    void loadWords();
    void pickTargetWord();
    void resetBoard();
    QStringList evaluateGuess(const QString &guess);
    void updateKeyboardLetter(const QChar &letter, const QString &state);
    int stateRank(const QString &state) const;
    void setMessage(const QString &message);
    void notifyBoardChanged();
    QVariantMap makeCell(const QString &letter, const QString &state, bool revealed) const;

    static constexpr int kWordLen = 5;
    static constexpr int kMaxRows = 6;

    QStringList m_allWords;
    QString m_targetWord;
    QString m_gameMode = QStringLiteral("normal");
    int m_currentRow = 0;
    int m_currentCol = 0;
    QString m_gameStatus;
    QString m_message;
    int m_boardRevision = 0;
    QVector<QVector<QVariantMap>> m_board;
    QMap<QString, QString> m_keyboardStates;
    QStringList m_submittedGuesses;
};

#endif // WORDLEGAME_H
