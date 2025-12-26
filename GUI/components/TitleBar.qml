import QtQuick

Rectangle {
    id: root

    height: 40

    color: "transparent"

    required property Window windowToControl
    required property real mainWindowRadius

    Item {
        anchors.fill: parent
        anchors.rightMargin: buttonRow.width

        DragHandler {
            onActiveChanged: {
                if (active) {
                    root.windowToControl.startSystemMove();
                }
            }
        }

        Row {
            anchors.left: parent.left
            anchors.leftMargin: 15
            anchors.verticalCenter: parent.verticalCenter
            spacing: 10

            Image {
                source: "qrc:/resources/icon.png"
                width: 20
                height: 20
            }

            Text {
                text: root.windowToControl.title
                color: "#ddd"

                style: Text.Raised
                styleColor: Qt.rgba(0, 0, 0)

                font.pixelSize: 18
                font.bold: true
            }
        }
    }

    Row {
        id: buttonRow
        anchors.right: parent.right
        height: parent.height

        // Minimize
        Rectangle {
            width: 45
            height: parent.height
            color: minMA.containsMouse ? '#4a4a4a' : "transparent"

            Text {
                text: "—"
                color: "white"
                anchors.centerIn: parent
            }

            MouseArea {
                id: minMA
                anchors.fill: parent
                hoverEnabled: true
                onClicked: root.windowToControl.showMinimized()
            }
        }

        // Close
        Rectangle {
            width: 45
            topRightRadius: root.mainWindowRadius
            height: parent.height
            color: closeMA.containsMouse ? "#e81123" : "transparent"

            Text {
                text: "✕"
                color: "white"
                anchors.centerIn: parent
            }

            MouseArea {
                id: closeMA
                anchors.fill: parent
                hoverEnabled: true
                onClicked: root.windowToControl.close()
            }
        }
    }
}
