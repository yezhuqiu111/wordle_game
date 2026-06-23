import QtQuick

Item {
    id: icon

    property bool lightMode: true
    property color iconColor: "#1a1a1b"
    property color cutoutColor: "#f0f0f0"

    implicitWidth: 22
    implicitHeight: 22

    // 浅色模式：显示月亮（点击切换深色）
    Item {
        visible: icon.lightMode
        anchors.centerIn: parent
        width: 20
        height: 20

        Rectangle {
            anchors.centerIn: parent
            width: 14
            height: 14
            radius: 7
            color: icon.iconColor
        }

        Rectangle {
            x: 8
            y: 3
            width: 12
            height: 12
            radius: 6
            color: icon.cutoutColor
        }
    }

    // 深色模式：显示太阳（点击切换浅色）
    Item {
        visible: !icon.lightMode
        anchors.centerIn: parent
        width: 22
        height: 22

        Rectangle {
            id: sunCore
            anchors.centerIn: parent
            width: 10
            height: 10
            radius: 5
            color: icon.iconColor
        }

        Repeater {
            model: 8

            Rectangle {
                width: 2
                height: 4
                radius: 1
                color: icon.iconColor

                property real angle: index * Math.PI / 4
                x: sunCore.x + sunCore.width / 2 - width / 2 + 9 * Math.cos(angle)
                y: sunCore.y + sunCore.height / 2 - height / 2 + 9 * Math.sin(angle)
            }
        }
    }
}
