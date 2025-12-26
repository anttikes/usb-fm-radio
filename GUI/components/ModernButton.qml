import QtQuick
import QtQuick.Controls

Button {
    id: root

    leftPadding: 20
    topPadding: 10
    rightPadding: 20
    bottomPadding: 10

    hoverEnabled: enabled ? true : false

    background: Rectangle {
        id: backgroundContainer

        radius: 8

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
            anchors.fill: parent
            anchors.margins: 2

            radius: 6

            gradient: Gradient {
                orientation: Gradient.Vertical

                GradientStop {
                    position: 0.0
                    color: !root.enabled ? '#50504E' : root.pressed ? '#3a3a38' : root.hovered ? '#8e8e8c' : '#767674'
                }

                GradientStop {
                    position: 0.5
                    color: !root.enabled ? '#3D3D3B' : root.pressed ? '#2f2f2d' : root.hovered ? '#5a5a58' : '#444442'
                }
            }
        }
    }
}
