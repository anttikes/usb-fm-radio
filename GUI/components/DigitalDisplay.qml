import QtQml
import QtQuick
import QtQuick.Layouts

Item {
    id: root

    width: 220
    height: 90

    property real currentFrequency: 0.0

    onCurrentFrequencyChanged: {
        if (currentFrequency === 0.0) {
            frequencyText.text = "";
            mhzText.text = "";
            return;
        }

        frequencyText.text = `${root.currentFrequency.toFixed(1)}`;
        mhzText.text = `MHz`;
    }

    FontLoader {
        id: lcdFont

        source: "qrc:/resources/DSEG7ClassicMini-Bold.ttf"
    }

    // Border, LCD-style frequency display
    Rectangle {
        anchors.fill: parent

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

            radius: 6

            gradient: Gradient {

                GradientStop {
                    position: 0.0
                    color: '#000000'
                }

                GradientStop {
                    position: 0.5
                    color: "#040D2B"
                }

                GradientStop {
                    position: 1.0
                    color: '#000000'
                }
            }

            GlowingText {
                id: frequencyText

                anchors.left: parent.left
                anchors.leftMargin: 30

                anchors.verticalCenter: parent.verticalCenter

                font.family: lcdFont.name
                font.pointSize: 34
            }

            GlowingText {
                id: mhzText

                anchors.left: frequencyText.right
                anchors.leftMargin: 3

                anchors.bottom: frequencyText.bottom

                font.bold: true
                font.pointSize: 14
            }
        }
    }
}
