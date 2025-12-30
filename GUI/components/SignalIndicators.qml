import QtQuick
import QtQuick.Layouts

ColumnLayout {
    id: root

    property alias receivedSignalStrength: signalStrength.value
    property alias receivedSignalStrengthUnit: signalStrength.unit

    property alias signalToNoise: signalToNoise.value
    property alias signalToNoiseUnit: signalToNoise.unit

    property alias signalStrengthMin: signalStrength.from
    property alias signalStrengthMax: signalStrength.to

    property alias signalToNoiseMin: signalToNoise.from
    property alias signalToNoiseMax: signalToNoise.to

    width: implicitWidth
    height: implicitHeight

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

        //Layout.topMargin: 5

        from: 0.0
        to: 127.0

        unit: 'dBµV'
    }

    Text {
        Layout.alignment: Qt.AlignLeft | Qt.AlignTop
        //Layout.topMargin: 5
        Layout.leftMargin: 8

        text: qsTr("Noise ratio")
        color: '#E4E4E3'
        font.pixelSize: 16
    }

    GradientBar {
        id: signalToNoise

        //Layout.topMargin: 5

        from: 0.0
        to: 127.0

        unit: 'dB'
    }
}
