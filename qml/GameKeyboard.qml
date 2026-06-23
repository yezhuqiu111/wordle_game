import QtQuick
import wordle_game

Column {
    id: keyboardRoot

    property var keyStates: ({})
    property bool lightMode: true
    property bool gameOver: false

    signal keyPressed(string key)

    spacing: 8

    Row {
        spacing: 6
        anchors.horizontalCenter: parent.horizontalCenter
        Repeater {
            model: ["Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P"]
            delegate: KeyButton {
                required property string modelData
                keyLabel: modelData
                keyStates: keyboardRoot.keyStates
                lightMode: keyboardRoot.lightMode
                gameOver: keyboardRoot.gameOver
                onTapped: function(k) { keyboardRoot.keyPressed(k) }
            }
        }
    }

    Row {
        spacing: 6
        anchors.horizontalCenter: parent.horizontalCenter
        Repeater {
            model: ["A", "S", "D", "F", "G", "H", "J", "K", "L"]
            delegate: KeyButton {
                required property string modelData
                keyLabel: modelData
                keyStates: keyboardRoot.keyStates
                lightMode: keyboardRoot.lightMode
                gameOver: keyboardRoot.gameOver
                onTapped: function(k) { keyboardRoot.keyPressed(k) }
            }
        }
    }

    Row {
        spacing: 6
        anchors.horizontalCenter: parent.horizontalCenter
        Repeater {
            model: ["ENTER", "Z", "X", "C", "V", "B", "N", "M", "BACK"]
            delegate: KeyButton {
                required property string modelData
                keyLabel: modelData
                keyStates: keyboardRoot.keyStates
                lightMode: keyboardRoot.lightMode
                gameOver: keyboardRoot.gameOver
                onTapped: function(k) { keyboardRoot.keyPressed(k) }
            }
        }
    }
}
