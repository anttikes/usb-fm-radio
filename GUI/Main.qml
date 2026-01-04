// main.qml
import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import QtQuick.Effects

import "./components"

Window {
    id: mainWindow

    minimumWidth: 1024
    minimumHeight: 640

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

        ColumnLayout {
            anchors.top: titleBar.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom

            anchors.topMargin: 10
            anchors.leftMargin: 20
            anchors.rightMargin: 20
            anchors.bottomMargin: 20

            spacing: 0

            TunerDial {
                Layout.fillWidth: true

                currentFrequency: 94.7
            }

            RowLayout {
                Layout.alignment: Qt.AlignHCenter

                ModernButton {
                    icon.source: "qrc:/resources/backward-solid-full.svg"

                    icon.width: 40
                    icon.height: 30

                    icon.color: hovered ? '#afd8f5' : '#ACD6EE'
                }

                DigitalDisplay {
                    id: digitalDisplay

                    currentFrequency: 94.7
                }

                ModernButton {
                    id: control

                    hoverEnabled: true

                    icon.source: "qrc:/resources/forward-solid-full.svg"

                    icon.width: 40
                    icon.height: 30

                    icon.color: hovered ? '#afd8f5' : '#ACD6EE'
                }
            }

            RDSPanel {
                Layout.fillWidth: true
                Layout.fillHeight: true

                stationName: "BBC R3"
                radioText: "Beethoven - Symphony No. 7"
            }

            RowLayout {
                Layout.alignment: Qt.AlignLeft

                SignalIndicators {
                    Layout.alignment: Qt.AlignLeft
                    Layout.topMargin: 10

                    receivedSignalStrength: 70
                    signalToNoise: 30
                }

                StereoIndicator {
                    Layout.leftMargin: 120
                    Layout.alignment: Qt.AlignHCenter

                    isStereo: true
                }

                Item {
                    Layout.fillWidth: true
                }

                PowerButton {
                    Layout.rightMargin: 20
                    Layout.alignment: Qt.AlignRight

                    hasPower: true
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
