import QtQuick
import QtQuick.Controls
import QtQuick.Effects
import QtQuick.Layouts

RowLayout {
    id: root

    property alias from: progressBar.from
    property alias to: progressBar.to
    property alias value: progressBar.value

    property string unit

    ProgressBar {
        id: progressBar

        padding: 4

        background: Rectangle {
            radius: 10

            gradient: Gradient {
                orientation: Gradient.Vertical

                GradientStop {
                    position: 0.0
                    color: '#262626'
                }

                GradientStop {
                    position: 1.0
                    color: '#4A4A4A'
                }
            }

            Rectangle {
                id: controlBackground

                anchors.fill: parent
                anchors.margins: 2

                radius: 6

                color: "#000000"
            }
        }

        contentItem: Item {
            implicitHeight: 30
            implicitWidth: 260

            Item {
                id: opacityMask

                anchors.fill: parent

                layer.enabled: true
                visible: false

                Rectangle {
                    width: progressBar.visualPosition * parent.width
                    height: parent.height

                    anchors.margins: 1

                    radius: 6
                }
            }

            Item {
                id: contentFill

                anchors.fill: parent

                visible: false

                Rectangle {
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.right: parent.right

                    height: parent.height / 2

                    gradient: Gradient {
                        orientation: Gradient.Horizontal

                        GradientStop {
                            position: 0.0
                            color: "#6F7AAF"
                        }
                        GradientStop {
                            position: 1.0
                            color: "#789B9F"
                        }
                    }
                }

                Rectangle {
                    anchors.bottom: parent.bottom
                    anchors.left: parent.left
                    anchors.right: parent.right

                    height: parent.height / 2

                    gradient: Gradient {
                        orientation: Gradient.Horizontal

                        GradientStop {
                            position: 0.0
                            color: "#454FAC"
                        }
                        GradientStop {
                            position: 1.0
                            color: "#789B9F"
                        }
                    }
                }
            }

            MultiEffect {
                anchors.fill: opacityMask

                source: contentFill

                maskEnabled: true
                maskSource: opacityMask
            }
        }
    }

    Text {
        id: text

        Layout.alignment: Qt.AlignVCenter | Qt.AlignRight

        text: root.unit ? `${progressBar.value} ${root.unit}` : `${progressBar.value}`

        font.pointSize: 12
        color: '#E4E4E3'
    }
}
