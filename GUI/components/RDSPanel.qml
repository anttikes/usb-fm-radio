import QtQuick
import QtQuick.Layouts

Item {
    id: root

    property string stationName: ""
    property string radioText: ""

    function resetDisplay() {
        stationName = "";
        radioText = "";

        updateDisplay();
    }

    function updateDisplay() {
        if (DeviceManager.selectedDeviceIndex >= 0) {
            if (stationName.length > 0 && radioText.length > 0) {
                topRow.text = stationName;
                secondRow.text = radioText;
            } else {
                topRow.text = qsTr("Waiting for RDS data...");
                secondRow.text = "";
            }
        } else {
            topRow.text = qsTr("No radios detected; please connect a radio device to your computer");
            secondRow.text = "";
        }
    }

    Component.onCompleted: {
        updateDisplay();
    }

    onStationNameChanged: {
        updateDisplay();
    }

    onRadioTextChanged: {
        updateDisplay();
    }

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
                color: '#3d3d3b'
            }

            GradientStop {
                position: 1.0
                color: '#4B4A48'
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
                    color: '#01050f'
                }

                GradientStop {
                    position: 1.0
                    color: '#000000'
                }
            }

            ColumnLayout {
                anchors.top: parent.top
                anchors.left: parent.left

                anchors.topMargin: 10
                anchors.leftMargin: 20

                spacing: 2

                GlowingText {
                    id: topRow

                    brightness: 0.4
                    blur: 0.1

                    font.bold: true
                    font.pointSize: 12
                }

                GlowingText {
                    id: secondRow

                    brightness: 0.4
                    blur: 0.1

                    font.bold: true
                    font.pointSize: 12
                }
            }
        }
    }
}
