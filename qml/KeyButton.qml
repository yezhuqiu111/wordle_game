import QtQuick

Rectangle {
    id: keyBtn

    property string keyLabel: "A"
    property var keyStates: ({})
    property bool lightMode: true
    property bool gameOver: false

    signal tapped(string key)

    readonly property bool isLetter: keyLabel.length === 1 && keyLabel >= "A" && keyLabel <= "Z"
    readonly property color correctColor: lightMode ? "#6aaa64" : "#538d4e"
    readonly property color presentColor: lightMode ? "#c9b458" : "#b59f3b"
    readonly property color absentColor: lightMode ? "#787c7e" : "#3a3a3c"
    readonly property color defaultKey: lightMode ? "#d3d6da" : "#818384"
    readonly property color defaultText: lightMode ? "#1a1a1b" : "#ffffff"

    implicitWidth: keyLabel === "ENTER" || keyLabel === "BACK" ? 65 : 43
    implicitHeight: 58
    radius: 4

    color: {
        if (!isLetter)
            return defaultKey
        const state = keyStates[keyLabel]
        if (state === "correct") return correctColor
        if (state === "present") return presentColor
        if (state === "absent") return absentColor
        return defaultKey
    }

    opacity: gameOver && keyLabel !== "ENTER" ? 0.7 : 1.0

    Behavior on color {
        ColorAnimation { duration: 200 }
    }

    Text {
        anchors.centerIn: parent
        text: keyLabel === "BACK" ? qsTr("删除") : keyLabel
        font.pixelSize: keyLabel === "ENTER" ? 12 : keyLabel === "BACK" ? 14 : 18
        font.bold: true
        font.family: "Microsoft YaHei UI"
        color: {
            if (!keyBtn.isLetter)
                return keyBtn.defaultText
            const state = keyStates[keyLabel]
            if (state === "correct" || state === "present" || state === "absent")
                return "#ffffff"
            return keyBtn.defaultText
        }
    }

    MouseArea {
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        onClicked: keyBtn.tapped(keyBtn.keyLabel)
    }
}
