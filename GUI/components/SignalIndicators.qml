import QtQuick
import QtQuick.Layouts

ColumnLayout {
    id: root

    property real receivedSignalStrength: 0.0
    property real signalToNoise: 0.0

    ColumnLayout {
        spacing: 0

        Text {
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            Layout.leftMargin: 8

            text: qsTr("Signal strength")
            color: '#E4E4E3'
            font.pixelSize: 16
        }

        GradientBar {
            id: signalStrength

            Layout.topMargin: 5

            from: 0.0
            to: 127.0

            value: 70
            unit: 'dBµV'
        }

        Text {
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            Layout.leftMargin: 8

            text: qsTr("Noise ratio")
            color: '#E4E4E3'
            font.pixelSize: 16
        }

        GradientBar {
            id: signalToNoise

            Layout.topMargin: 5

            from: 0.0
            to: 127.0

            value: 30
            unit: 'dB'
        }
    }
}
