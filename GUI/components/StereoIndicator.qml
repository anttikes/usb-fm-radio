import QtQuick
import QtQuick.Effects
import QtQuick.Layouts

ColumnLayout {
    id: root

    property bool isStereo

    width: implicitWidth
    height: implicitHeight

    spacing: 0

    Text {
        id: titleText

        Layout.alignment: Qt.AlignLeft | Qt.AlignTop
        Layout.leftMargin: 8

        text: qsTr("Stereo")
        color: '#E4E4E3'

        font.pixelSize: 16
    }

    // Outer border
    Rectangle {
        Layout.alignment: Qt.AlignLeft | Qt.AlignTop
        Layout.topMargin: 5

        implicitWidth: innerBorder.implicitWidth + innerBorder.anchors.margins * 2
        implicitHeight: innerBorder.implicitHeight + innerBorder.anchors.margins * 2

        radius: 10

        gradient: Gradient {
            orientation: Gradient.Vertical

            GradientStop {
                position: 0.0
                color: '#272525'
            }

            GradientStop {
                position: 1.0
                color: '#413F3D'
            }
        }

        // Inner border, provides a "sunken" effect
        Rectangle {
            id: innerBorder

            implicitWidth: stereoImage.implicitWidth + stereoImage.anchors.margins * 2 + anchors.margins * 2
            implicitHeight: stereoImage.implicitHeight + stereoImage.anchors.margins * 2 + anchors.margins * 2

            anchors.fill: parent
            anchors.margins: 7

            radius: 6

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

            Image {
                id: stereoImage

                anchors.centerIn: parent
                anchors.margins: 4

                source: "qrc:/resources/StereoIndicator.png"
                sourceSize.width: 42
                fillMode: Image.PreserveAspectFit
                visible: false
            }

            MultiEffect {
                id: effect

                anchors.fill: stereoImage

                source: stereoImage

                // Bloom effect
                shadowEnabled: true
                shadowColor: root.isStereo ? "#A8D6F2" : '#fd0000'
                shadowBlur: 0.8
                shadowHorizontalOffset: 0
                shadowVerticalOffset: 0

                // Intensity
                brightness: root.isStereo ? 0.3 : 0.5
                contrast: 0.2

                // Adding a slight blur to the text itself makes it
                // look less like "pixels" and more like "light"
                blurEnabled: true
                blur: 0.1
            }
        }
    }
}
