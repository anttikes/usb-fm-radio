import QtQuick
import QtQuick.Controls

ApplicationWindow {
    id: mainWindow
    width: 640
    height: 480
    visible: true
    title: qsTr("USB FM Radio")

    color: "transparent"

    flags: Qt.Window | Qt.FramelessWindowHint

    // Border and backdrop
    Rectangle {
        id: glassBorder
        anchors.fill: parent

        color: Qt.rgba(0.34, 0.34, 0.34)
        border.color: Qt.rgba(0, 0, 0)
        border.width: 1
        radius: 10

        // Title bar
        Rectangle {
            anchors.top: parent.top
            anchors.left: parent.left
            width: parent.width
            height: 35
            color: "transparent"
            radius: 10

            DragHandler {
                onActiveChanged: {
                    if (active) {
                        mainWindow.startSystemMove();
                    }
                }
            }

            Image {
                id: appIcon
                source: "qrc:/images/icon.png"
                width: 20
                height: 20
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: 10
            }

            Text {
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: appIcon.right
                anchors.leftMargin: 5
                color: Qt.rgba(0.73, 0.73, 0.73)
                text: mainWindow.title
                font.pixelSize: 16
            }

            Row {
                anchors.bottom: parent.bottom
                anchors.right: parent.right
                height: parent.height - glassBorder.border.width

                // Minimize Button
                Rectangle {
                    width: 45
                    height: parent.height
                    color: minMouse.containsMouse ? '#808080' : "transparent"
                    Text {
                        text: "—"
                        anchors.centerIn: parent
                        color: Qt.rgba(1, 1, 1)
                    }
                    MouseArea {
                        id: minMouse
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: mainWindow.showMinimized()
                    }
                }

                // Close Button
                Rectangle {
                    width: 45
                    topRightRadius: 10
                    height: parent.height
                    color: closeMouse.containsMouse ? "#e81123" : "transparent"

                    Text {
                        text: "✕"
                        anchors.centerIn: parent
                        color: Qt.rgba(1, 1, 1)
                    }

                    MouseArea {
                        id: closeMouse
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: mainWindow.close()
                    }
                }
            }
        }

        // Main Content Area (Gradient Background)
        Rectangle {
            anchors.fill: parent
            anchors.topMargin: 35
            anchors.leftMargin: 10
            anchors.bottomMargin: 10
            anchors.rightMargin: 10
            radius: 8

            gradient: Gradient {
                GradientStop {
                    position: 0.0
                    color: "#646664"
                }
                GradientStop {
                    position: 1.0
                    color: "#303030"
                }
            }
        }
    }
}
