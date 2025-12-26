// main.qml
import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import QtQuick.Effects

import "./components"

Window {
    id: mainWindow
    width: 1024
    height: 640
    visible: true
    title: qsTr("USB FM Radio")

    // Remove native borders and make background transparent so our custom shapes show
    flags: Qt.Window | Qt.FramelessWindowHint
    color: "transparent"

    // Main window background
    Rectangle {
        id: appBackground
        anchors.fill: parent
        radius: 12
        border.color: "#33FFFFFF"
        border.width: 1

        gradient: Gradient {
            orientation: Gradient.Vertical

            GradientStop {
                position: 0.0
                color: "#5F615F"
            }

            GradientStop {
                position: 1.0
                color: "#2F2F2F"
            }
        }

        TitleBar {
            id: titleBar

            anchors.left: parent.left
            anchors.top: parent.top
            anchors.right: parent.right

            mainWindowRadius: appBackground.radius
            windowToControl: mainWindow
        }

        TunerDial {
            id: tunerDial

            anchors.top: titleBar.bottom
            anchors.left: parent.left
            anchors.right: parent.right

            anchors.topMargin: 10
            anchors.leftMargin: 20
            anchors.rightMargin: 20

            currentFrequency: 94.7
        }

        DigitalDisplay {
            id: digitalDisplay

            anchors.top: tunerDial.bottom
            anchors.topMargin: 5
            anchors.horizontalCenter: tunerDial.horizontalCenter

            currentFrequency: 94.7
        }

        ColumnLayout {
            anchors.top: tunerDial.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom

            spacing: 0

            ColumnLayout {
                Layout.margins: 20
                spacing: 15

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 20

                    ModernButton {}

                    ModernButton {}
                }

                // RDS Information Panel
                RDSPanel {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 120

                    stationName: "BBC R3"
                    progType: "Classical"
                    radioText: "Now Playing: Beethoven - Symphony No. 7"
                }
            }
        }
    }

    // Drop shadow effect for the main background
    MultiEffect {
        source: appBackground
        anchors.fill: appBackground

        // Mask; the main window border has rounded corners
        maskEnabled: true

        // Shadow Properties
        shadowEnabled: true
        shadowColor: "#000000"
        shadowBlur: 1.0
        shadowHorizontalOffset: 5
        shadowVerticalOffset: 5

        // Performance & Padding
        autoPaddingEnabled: true
        blurMax: 64
    }
}
