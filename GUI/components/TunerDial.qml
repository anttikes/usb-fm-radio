import QtQml
import QtQuick
import QtQuick.Controls
import QtQuick.Effects
import QtQuick.Layouts
import QtQuick.Shapes

Item {
    id: root

    height: 95

    property real currentFrequency

    readonly property real minFreq: 87.5
    readonly property real maxFreq: 108.0

    property ListModel tickModel: ListModel {}

    Component.onCompleted: {
        for (var freq = root.minFreq; freq <= root.maxFreq; freq += 0.25) {
            if ((freq % 2) === 0) {
                // At every even 2 MHz there's a large tick with a frequency value
                tickModel.append({
                    "tickFrequency": freq,
                    "tickHeight": 19,
                    "tickWidth": 2
                });
            } else {
                // Otherwise at every 0.25 MHz there's a small tick; it also has frequency but the value is not drawn
                tickModel.append({
                    "tickFrequency": freq,
                    "tickHeight": 11,
                    "tickWidth": 1
                });
            }
        }
    }

    onCurrentFrequencyChanged: {
        console.info("Current frequency updated from the outside: " + root.currentFrequency);
        frequencySlider.value = root.currentFrequency;
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

            Slider {
                id: frequencySlider

                anchors.fill: parent
                anchors.leftMargin: 10
                anchors.rightMargin: 10

                from: root.minFreq
                to: root.maxFreq

                stepSize: 0.1
                live: false

                onValueChanged: {
                    console.info("Frequency slider was dragged to: " + frequencySlider.value);
                }

                background: Rectangle {
                    x: frequencySlider.leftPadding
                    y: frequencySlider.topPadding

                    width: frequencySlider.availableWidth
                    height: frequencySlider.height

                    color: "transparent"

                    // Tick lines in a row layout
                    RowLayout {
                        anchors.fill: parent

                        anchors.leftMargin: 6
                        anchors.topMargin: 4
                        anchors.rightMargin: 1
                        anchors.bottomMargin: 4

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

                        anchors.top: parent.top
                        anchors.left: parent.left
                        anchors.right: parent.right
                        height: 22

                        anchors.topMargin: 4
                        anchors.leftMargin: -8
                        anchors.rightMargin: -8

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

                handle: Rectangle {
                    x: frequencySlider.leftPadding + frequencySlider.visualPosition * (frequencySlider.availableWidth - width)
                    y: frequencySlider.topPadding + frequencySlider.availableHeight / 2 - height / 2

                    width: 13
                    height: frequencySlider.availableHeight

                    color: 'transparent'

                    Shape {
                        id: containerShape
                        anchors.fill: parent

                        visible: false

                        ShapePath {
                            strokeWidth: 0

                            fillGradient: LinearGradient {
                                x1: 0
                                y1: 0
                                x2: containerShape.width
                                y2: containerShape.height

                                GradientStop {
                                    position: 0.0
                                    color: "#DF9C68"
                                }
                                GradientStop {
                                    position: 1.0
                                    color: "#E1A96C"
                                }
                            }

                            startX: 0
                            startY: 0

                            // Top edge
                            PathLine {
                                x: containerShape.width
                                y: 0
                            }

                            // Down towards the tip and center
                            PathLine {
                                x: (containerShape.width / 2) + containerShape.width * 0.1
                                y: containerShape.height * 0.15
                            }

                            // Straight down
                            PathLine {
                                x: (containerShape.width / 2) + containerShape.width * 0.1
                                y: containerShape.height * 0.85
                            }

                            // Down towards the bottom right corner
                            PathLine {
                                x: containerShape.width
                                y: containerShape.height
                            }

                            // Bottom edge
                            PathLine {
                                x: 0
                                y: containerShape.height
                            }

                            // Up towards the tip and center
                            PathLine {
                                x: (containerShape.width / 2) - containerShape.width * 0.1
                                y: containerShape.height * 0.85
                            }

                            // Straight up
                            PathLine {
                                x: (containerShape.width / 2) - containerShape.width * 0.1
                                y: containerShape.height * 0.15
                            }

                            // Up towards the top left corner
                            PathLine {
                                x: 0
                                y: 0
                            }
                        }
                    }

                    MultiEffect {
                        source: containerShape
                        anchors.fill: containerShape

                        // Bloom effect
                        shadowEnabled: true
                        shadowColor: "#FFDAB9"
                        shadowBlur: 0.6
                        shadowHorizontalOffset: 0
                        shadowVerticalOffset: 0

                        // Intensity
                        brightness: 0.4
                        contrast: 0.2

                        // Blurring
                        blurEnabled: true
                        blur: 0.3
                    }
                }
            }
        }
    }
}
