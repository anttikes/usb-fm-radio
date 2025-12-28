import QtQml
import QtQuick
import QtQuick.Controls
import QtQuick.Effects
import QtQuick.Layouts

Item {
    id: root

    height: 90

    required property real currentFrequency

    readonly property real minFreq: 87.5
    readonly property real maxFreq: 108.0

    property ListModel tickModel: ListModel {}

    Component.onCompleted: {
        for (var freq = root.minFreq; freq <= root.maxFreq; freq += 0.25) {
            if ((freq % 2) === 0) {
                // At every even 2 MHz there's a large tick with a frequency value
                tickModel.append({
                    "tickFrequency": freq,
                    "tickHeight": tickArea.height * 0.8,
                    "tickWidth": 2
                });
            } else {
                // Otherwise at every 0.25 MHz there's a small tick; it also has frequency but the value is not drawn
                tickModel.append({
                    "tickFrequency": freq,
                    "tickHeight": tickArea.height * 0.4,
                    "tickWidth": 1
                });
            }
        }
    }

    Text {
        id: titleText

        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 8

        text: qsTr("Frequency")
        color: '#E4E4E3'

        font.pixelSize: 16
    }

    // Border, frequency dial
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
                color: '#3d3d3b'
            }

            GradientStop {
                position: 1.0
                color: '#61615e'
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

            // Tick lines in a row layout
            RowLayout {
                anchors.top: tickArea.top
                anchors.left: tickArea.left
                anchors.right: tickArea.right
                anchors.bottom: parent.bottom

                anchors.leftMargin: 10
                anchors.rightMargin: 15

                Repeater {
                    model: root.tickModel

                    delegate: Item {
                        id: delegateRoot

                        required property real tickWidth
                        required property real tickHeight
                        required property real tickFrequency

                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        Rectangle {
                            anchors.bottom: parent.verticalCenter

                            width: delegateRoot.tickWidth
                            height: delegateRoot.tickHeight

                            color: Qt.rgba(1.0, 1.0, 1.0)
                        }

                        Text {
                            anchors.top: parent.verticalCenter
                            anchors.horizontalCenter: parent.horizontalCenter

                            visible: delegateRoot.tickWidth > 1

                            color: Qt.rgba(0.66, 0.84, 0.95)

                            text: delegateRoot.tickFrequency.toFixed(1)
                        }
                    }
                }
            }

            // Ticker area; when drawn on top of the tickers provides the distinctive "tint"
            Rectangle {
                id: tickArea

                anchors.margins: 6
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                height: 21

                radius: 5

                gradient: Gradient {
                    orientation: Gradient.Horizontal

                    GradientStop {
                        position: 0.0
                        color: '#b7313131'
                    }

                    GradientStop {
                        position: 0.5
                        color: '#b733394e'
                    }

                    GradientStop {
                        position: 1.0
                        color: '#b7313131'
                    }
                }
            }
        }
    }
}
