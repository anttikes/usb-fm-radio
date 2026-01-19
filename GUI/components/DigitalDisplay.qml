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

            RowLayout {
                anchors.fill: parent
                anchors.rightMargin: 10

                spacing: 3

                GlowingText {
                    id: frequencyText

                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter

                    font.family: lcdFont.name
                    font.pointSize: 34
                }

                GlowingText {
                    id: mhzText
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                    Layout.bottomMargin: -20

                    font.bold: true
                    font.pointSize: 14
                }
            }
        }
    }
}
