import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import wordle_game

ApplicationWindow {
    id: window

    width: 520
    height: 880
    minimumWidth: 400
    minimumHeight: 800
    visible: true
    title: qsTr("Wordle 猜词")

    property bool lightMode: Application.styleHints.colorScheme !== Qt.Dark

    readonly property color bgColor: lightMode ? "#ffffff" : "#121213"
    readonly property color headerColor: lightMode ? "#1a1a1b" : "#ffffff"
    readonly property color subtextColor: lightMode ? "#787c7e" : "#818384"
    readonly property color dividerColor: lightMode ? "#e0e0e0" : "#3a3a3c"
    readonly property color accentColor: lightMode ? "#6aaa64" : "#538d4e"

    color: bgColor

    FocusScope {
        id: keyScope
        focus: true
        width: 0
        height: 0

        Keys.onPressed: function(event) {
            if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
                wordleGame.submitGuess()
                event.accepted = true
            } else if (event.key === Qt.Key_Backspace) {
                wordleGame.removeLetter()
                event.accepted = true
            } else if (event.text.length === 1) {
                const ch = event.text.toUpperCase()
                if (ch >= "A" && ch <= "Z") {
                    wordleGame.addLetter(ch)
                    event.accepted = true
                }
            }
        }
    }

    Component.onCompleted: keyScope.forceActiveFocus()

    Connections {
        target: wordleGame
        function onInvalidGuess() {
            gameBoard.shakeRow = wordleGame.currentRow
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 4

        Item {
            Layout.fillWidth: true
            Layout.preferredHeight: 52

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                text: "WORDLE"
                font.pixelSize: 28
                font.bold: true
                font.family: "Microsoft YaHei UI"
                font.letterSpacing: 4
                color: headerColor
            }

            Rectangle {
                id: themeButton
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                width: 40
                height: 40
                radius: 8
                color: lightMode ? "#f0f0f0" : "#2a2a2c"

                ThemeIcon {
                    anchors.centerIn: parent
                    lightMode: window.lightMode
                    iconColor: headerColor
                    cutoutColor: themeButton.color
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: window.lightMode = !window.lightMode
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 1
            color: dividerColor
        }

        ModeSelector {
            Layout.fillWidth: true
            Layout.topMargin: 6
            Layout.bottomMargin: 2
            lightMode: window.lightMode
            currentMode: wordleGame.gameMode
            onModeSelected: function(mode) { wordleGame.setGameMode(mode) }
        }

        Text {
            Layout.fillWidth: true
            Layout.topMargin: 4
            Layout.bottomMargin: 6
            text: wordleGame.message !== ""
                  ? wordleGame.message
                  : (wordleGame.gameStatus === "playing" ? wordleGame.currentRowText : "")
            font.pixelSize: 14
            font.family: "Microsoft YaHei UI"
            color: wordleGame.message !== "" ? "#b59f3b" : subtextColor
            horizontalAlignment: Text.AlignHCenter
            height: 22
        }

        Item {
            Layout.fillWidth: true
            Layout.preferredHeight: gameBoard.implicitHeight

            GameBoard {
                id: gameBoard
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                lightMode: window.lightMode
            }
        }

        GameKeyboard {
            Layout.fillWidth: true
            Layout.preferredHeight: 200
            Layout.topMargin: 4
            Layout.bottomMargin: 4
            lightMode: window.lightMode
            keyStates: wordleGame.keyboardStates
            gameOver: wordleGame.gameStatus !== "playing"

            onKeyPressed: function(key) {
                keyScope.forceActiveFocus()
                if (key === "ENTER")
                    wordleGame.submitGuess()
                else if (key === "BACK")
                    wordleGame.removeLetter()
                else
                    wordleGame.addLetter(key)
            }
        }
    }

    GameResultDialog {
        lightMode: window.lightMode
        gameStatus: wordleGame.gameStatus
        answerWord: wordleGame.answerWord
        onPlayAgain: wordleGame.newGame()
    }
}