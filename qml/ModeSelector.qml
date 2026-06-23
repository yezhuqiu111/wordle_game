import QtQuick
import QtQuick.Layouts

ColumnLayout {
    id: selector

    property string currentMode: "normal"
    property bool lightMode: true

    signal modeSelected(string mode)

    readonly property color chipBg: lightMode ? "#f0f0f0" : "#2a2a2c"
    readonly property color chipActive: lightMode ? "#6aaa64" : "#538d4e"
    readonly property color chipText: lightMode ? "#1a1a1b" : "#ffffff"
    readonly property color chipTextActive: "#ffffff"
    readonly property color chipBorder: lightMode ? "#d3d6da" : "#3a3a3c"
    readonly property color subtextColor: lightMode ? "#787c7e" : "#a0a0a0"
    readonly property color accentColor: lightMode ? "#6aaa64" : "#538d4e"

    readonly property string modeHint: {
        switch (wordleGame.gameMode) {
        case "daily":
            return wordleGame.dailyDate
        default:
            return qsTr("经典模式：单机随机单词")
        }
    }

    spacing: 4

    RowLayout {
        Layout.fillWidth: true
        spacing: 8

        Repeater {
            model: wordleGame.availableModes

            Rectangle {
                required property string modelData

                property bool isActive: selector.currentMode === modelData

                Layout.fillWidth: true
                Layout.preferredHeight: 36
                radius: 8
                color: isActive ? chipActive : chipBg
                border.color: isActive ? chipActive : chipBorder
                border.width: 1

                Text {
                    anchors.centerIn: parent
                    text: wordleGame.modeDisplayName(modelData)
                    font.pixelSize: 13
                    font.bold: parent.isActive
                    font.family: "Microsoft YaHei UI"
                    color: parent.isActive ? chipTextActive : chipText
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: selector.modeSelected(modelData)
                }
            }
        }
    }

    Text {
        Layout.fillWidth: true
        Layout.preferredHeight: 22
        text: modeHint
        font.pixelSize: 15
        font.family: "Microsoft YaHei UI"
        color: wordleGame.gameMode === "daily" ? accentColor : subtextColor
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
}
