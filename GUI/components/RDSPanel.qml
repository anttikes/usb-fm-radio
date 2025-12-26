import QtQuick
import QtQuick.Layouts

Item {
    id: root

    property alias stationName: stationNameControl.text
    property alias radioText: radioTextControl.text

    Text {
        id: titleText

        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 8

        text: qsTr("RDS Information")
        color: '#E4E4E3'

        font.pixelSize: 16
    }

    // Outer border
    Rectangle {
        anchors.left: parent.left
        anchors.top: titleText.bottom
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        anchors.topMargin: 7

        radius: 10

        gradient: Gradient {
            orientation: Gradient.Vertical

            GradientStop {
                position: 0.0
                color: Qt.rgba(0.24, 0.24, 0.23)
            }

            GradientStop {
                position: 1.0
                color: Qt.rgba(0.38, 0.38, 0.37)
            }
        }

        // Inner border, provides a "sunken" effect
        Rectangle {
            anchors.fill: parent
            anchors.margins: 7

            gradient: Gradient {

                GradientStop {
                    position: 0.0
                    color: '#000000'
                }

                GradientStop {
                    position: 0.5
                    color: '#01050f'
                }

                GradientStop {
                    position: 1.0
                    color: '#000000'
                }
            }

            radius: 6

            ColumnLayout {
                anchors.top: parent.top
                anchors.left: parent.left

                anchors.topMargin: 10
                anchors.leftMargin: 20

                spacing: 2

                GlowingText {
                    id: stationNameControl

                    brightness: 0.4
                    blur: 0.1

                    font.bold: true
                    font.pointSize: 12
                }

                GlowingText {
                    id: radioTextControl

                    brightness: 0.4
                    blur: 0.1

                    font.bold: true
                    font.pointSize: 12
                }
            }
        }
    }
}
