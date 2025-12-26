import QtQuick
import QtQuick.Layouts

RowLayout {
    id: root

    spacing: 20
    property real signalStrength: 0.0
    property bool isStereo: false

    // --- Helper Component for the "GroupBox" style ---
    component GroupBoxFrame: Item {
        id: gbRoot
        default property alias content: container.data
        property string label: ""
        Layout.fillWidth: true
        Layout.fillHeight: true

        // The Border Frame
        Rectangle {
            id: frame
            anchors.fill: parent
            anchors.topMargin: 8
            color: "transparent"
            border.color: "#555"
            radius: 6
            // The Content Container
            Item {
                id: container
                anchors.fill: parent
                anchors.margins: 10
            }
        }

        // The "Cut-through" Label Mask
        Rectangle {
            anchors.left: frame.left
            anchors.leftMargin: 10
            anchors.verticalCenter: frame.top
            width: labelTxt.width + 8
            height: labelTxt.height
            // IMPORTANT: Match this color to the main window background gradient approximate color at this position
            color: "#1f2a36"
            Text {
                id: labelTxt
                anchors.centerIn: parent
                text: gbRoot.label
                color: "#aaa"
                font.pixelSize: 11
            }
        }
    }

    // 1. Signal Strength Indicator
    GroupBoxFrame {
        label: "SIGNAL STRENGTH"
        RowLayout {
            spacing: 10
            // The Layered Progress Bar
            Rectangle {
                Layout.fillWidth: true
                height: 20
                color: "#050505"
                radius: 3
                clip: true
                // Background shadow layer
                Rectangle {
                    width: parent.width * root.signalStrength
                    height: parent.height
                    opacity: 0.3
                    color: "#3a7bd5"
                }
                // Foreground active layer (thinner)
                Rectangle {
                    width: parent.width * root.signalStrength
                    height: 12
                    anchors.verticalCenter: parent.verticalCenter
                    gradient: Gradient {
                        GradientStop {
                            position: 0.0
                            color: "#00d2ff"
                        }
                        GradientStop {
                            position: 1.0
                            color: "#3a7bd5"
                        }
                    }
                }
            }
            Text {
                text: Math.round(root.signalStrength * 100) + "%"
                color: "white"
            }
        }
    }

    // 2. Stereo Indicator
    GroupBoxFrame {
        label: "STEREO"
        Layout.preferredWidth: 100

        // The Stereo Icon (Simulated interlocking rings)
        Item {
            anchors.centerIn: parent
            width: 40
            height: 25
            opacity: root.isStereo ? 1.0 : 0.3 // Dim if not stereo

            // Left Ring
            Rectangle {
                width: 25
                height: 25
                radius: 12.5
                color: "transparent"
                border.color: "#00d2ff"
                border.width: 3
            }
            // Right Ring (overlapping)
            Rectangle {
                x: 15
                width: 25
                height: 25
                radius: 12.5
                color: "transparent"
                border.color: "#00d2ff"
                border.width: 3
            }
            // Visual Trick: A glow effect behind the rings if active
            Rectangle {
                anchors.fill: parent
                visible: root.isStereo
                color: "#00d2ff"
                opacity: 0.2
                radius: 12.5
            }
        }
    }
}
