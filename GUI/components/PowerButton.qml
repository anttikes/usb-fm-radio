import QtQuick
import QtQuick.Controls

Button {
    id: root

    property bool hasPower

    implicitWidth: 70
    implicitHeight: implicitWidth

    hoverEnabled: enabled ? true : false

    topPadding: pressed ? 1 : 0

    icon.source: "qrc:/resources/PowerButton.png"

    icon.width: 32
    icon.height: 32

    icon.color: !hasPower && pressed ? Qt.darker('#708998', 1.2) :  // Power off, pressed
    !hasPower && hovered ? Qt.lighter('#708998', 1.2) :             // Power off, hovered
    !hasPower ? '#708998' :                                         // Power off, default
    hasPower && pressed ? Qt.darker('#ACD6EE', 1.2) :               // Power on, pressed
    hasPower && hovered ? Qt.lighter('#ACD6EE', 1.2) :              // Power on, hovered
    '#ACD6EE'                                                       // Power on, default

    background: Rectangle {
        radius: width / 2

        gradient: Gradient {
            orientation: Gradient.Vertical

            GradientStop {
                position: 0.0
                color: '#101010'
            }

            GradientStop {
                position: 1.0
                color: '#000000'
            }
        }

        Rectangle {
            id: raisedEdge

            anchors.fill: parent
            anchors.margins: 2

            radius: width / 2

            gradient: Gradient {
                orientation: Gradient.Vertical

                GradientStop {
                    position: 0.0
                    color: root.pressed ? '#151515' : '#989896'
                }

                GradientStop {
                    position: 1.0
                    color: root.pressed ? '#353535' : '#1F1F1D'
                }
            }

            Rectangle {
                id: topArea

                anchors.fill: parent

                anchors.topMargin: root.pressed ? 5 : 4
                anchors.bottomMargin: root.pressed ? 3 : 4
                anchors.leftMargin: 4
                anchors.rightMargin: 4

                radius: width / 2

                gradient: Gradient {
                    orientation: Gradient.Vertical

                    GradientStop {
                        position: 0.0
                        color: !root.enabled ? '#50504E' : // Disabled
                        root.pressed ? '#2e2e2e' :         // Pressed
                        root.hovered ? '#8E8E8C' :         // Hovered
                        '#767674'                          // Default
                    }

                    GradientStop {
                        position: 0.5
                        color: !root.enabled ? '#3D3D3B' : // Disabled
                        root.pressed ? '#3e3e3b' :         // Pressed
                        root.hovered ? '#5A5A58' :         // Hovered
                        '#444442'                          // Default
                    }

                    GradientStop {
                        position: 1.0
                        color: !root.enabled ? '#2A2A28' : // Disabled
                        root.pressed ? '#51514e' :         // Pressed
                        root.hovered ? '#4A4A48' :         // Hovered
                        '#333331'                          // Default
                    }
                }
            }
        }
    }
}
