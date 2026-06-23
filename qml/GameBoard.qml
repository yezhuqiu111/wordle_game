import QtQuick
import wordle_game

Column {
    id: boardRoot

    property bool lightMode: true
    property int shakeRow: -1

    spacing: 6

    Repeater {
        model: 6

        Row {
            id: boardRow
            required property int index

            spacing: 6
            anchors.horizontalCenter: parent.horizontalCenter

            transform: Translate {
                id: rowShake
                x: 0
            }

            SequentialAnimation {
                running: boardRoot.shakeRow === boardRow.index
                loops: 1
                onStopped: boardRoot.shakeRow = -1

                NumberAnimation { target: rowShake; property: "x"; to: -8; duration: 50 }
                NumberAnimation { target: rowShake; property: "x"; to: 8; duration: 50 }
                NumberAnimation { target: rowShake; property: "x"; to: -6; duration: 50 }
                NumberAnimation { target: rowShake; property: "x"; to: 6; duration: 50 }
                NumberAnimation { target: rowShake; property: "x"; to: 0; duration: 50 }
            }

            Repeater {
                model: 5

                LetterTile {
                    id: tile

                    required property int index
                    readonly property int tileRow: boardRow.index
                    readonly property int tileCol: index

                    lightMode: boardRoot.lightMode
                    revealDelay: tileCol * 280

                    function refresh() {
                        letter = wordleGame.cellLetter(tileRow, tileCol)
                        cellState = wordleGame.cellState(tileRow, tileCol)
                    }

                    Component.onCompleted: refresh()

                    Connections {
                        target: wordleGame
                        function onBoardChanged() {
                            tile.refresh()
                        }
                    }
                }
            }
        }
    }
}
