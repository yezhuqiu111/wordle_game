import QtQuick
import QtQuick.Layouts

Item {
    id: overlay

    anchors.fill: parent
    z: 100
    visible: gameStatus === "won" || gameStatus === "lost"

    property bool lightMode: true
    property string gameStatus: "playing"
    property string answerWord: ""

    signal playAgain()

    Rectangle {
        anchors.fill: parent
        color: lightMode ? "#66000000" : "#99000000"

        MouseArea {
            anchors.fill: parent
            onClicked: {}
        }
    }

    Rectangle {
        id: dialogCard
        anchors.centerIn: parent
        width: Math.min(overlay.width - 48, 320)
        height: dialogColumn.height + 48
        radius: 16
        color: lightMode ? "#ffffff" : "#1f1f22"
        border.color: lightMode ? "#e0e0e0" : "#3a3a3c"
        border.width: 1
        z: 1

        Rectangle {
            z: -1
            anchors.fill: parent
            anchors.margins: -4
            radius: 18
            color: lightMode ? "#20000000" : "#40000000"
        }

        Column {
            id: dialogColumn
            anchors.centerIn: parent
            width: parent.width - 48
            spacing: 14

            Text {
                width: parent.width
                text: gameStatus === "won" ? qsTr("恭喜，猜对了！") : qsTr("下次再试吧")
                font.pixelSize: 22
                font.bold: true
                font.family: "Microsoft YaHei UI"
                color: lightMode ? "#1a1a1b" : "#ffffff"
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
            }

            Text {
                width: parent.width
                visible: gameStatus === "lost"
                text: qsTr("答案是 %1").arg(answerWord)
                font.pixelSize: 16
                font.family: "Microsoft YaHei UI"
                color: lightMode ? "#787c7e" : "#a0a0a0"
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
            }

            Rectangle {
                anchors.horizontalCenter: parent.horizontalCenter
                width: playAgainText.width + 32
                height: 44
                radius: 8
                color: lightMode ? "#6aaa64" : "#538d4e"

                Text {
                    id: playAgainText
                    anchors.centerIn: parent
                    text: qsTr("再玩一次")
                    font.pixelSize: 16
                    font.bold: true
                    font.family: "Microsoft YaHei UI"
                    color: "#ffffff"
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: overlay.playAgain()
                }
            }
        }
    }
}
