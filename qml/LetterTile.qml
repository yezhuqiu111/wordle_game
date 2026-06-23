import QtQuick

Rectangle {
    id: tile

    property string letter: ""
    property string cellState: "empty"
    property bool isRevealed: false // animated locally on submit
    property bool lightMode: true
    property int revealDelay: 0

    width: 62
    height: 62
    radius: 4
    border.width: 2

    readonly property color correctColor: lightMode ? "#6aaa64" : "#538d4e"
    readonly property color presentColor: lightMode ? "#c9b458" : "#b59f3b"
    readonly property color absentColor: lightMode ? "#787c7e" : "#3a3a3c"
    readonly property color emptyBorder: lightMode ? "#d3d6da" : "#3a3a3c"
    readonly property color filledBorder: lightMode ? "#878a8c" : "#565758"
    readonly property color tileBackground: lightMode ? "#ffffff" : "#121213"
    readonly property color textColor: isRevealed ? "#ffffff"
                                               : (lightMode ? "#1a1a1b" : "#ffffff")

    color: {
        if (!isRevealed)
            return letter !== "" ? tileBackground : "transparent"
        switch (cellState) {
        case "correct": return correctColor
        case "present": return presentColor
        case "absent": return absentColor
        default: return tileBackground
        }
    }

    border.color: {
        if (isRevealed)
            return "transparent"
        return letter !== "" ? filledBorder : emptyBorder
    }

  Behavior on color {
        ColorAnimation { duration: 250; easing.type: Easing.InOutQuad }
    }

    Behavior on border.color {
        ColorAnimation { duration: 200 }
    }

    scale: popAnimation.running ? 1.08 : 1.0

    Text {
        anchors.centerIn: parent
        text: tile.letter
        font.pixelSize: 34
        font.bold: true
        font.family: "Microsoft YaHei UI"
        font.letterSpacing: 1
        color: tile.textColor

        Behavior on color {
            ColorAnimation { duration: 200 }
        }
    }

    SequentialAnimation {
        id: popAnimation
        NumberAnimation { target: tile; property: "scale"; to: 1.08; duration: 80 }
        NumberAnimation { target: tile; property: "scale"; to: 1.0; duration: 80 }
    }

    Timer {
        id: revealTimer
        interval: revealDelay
        onTriggered: tile.isRevealed = true
    }

    onLetterChanged: {
        if (letter !== "" && !isRevealed)
            popAnimation.restart()
    }

    onCellStateChanged: {
        if (cellState === "empty" || cellState === "filled") {
            revealTimer.stop()
            isRevealed = false
        } else if (cellState === "correct" || cellState === "present" || cellState === "absent") {
            revealTimer.interval = revealDelay
            revealTimer.restart()
        }
    }
}
