import QtQuick
import QtQuick.Layouts

ColumnLayout {
    id: root

    property alias receivedSignalStrengthMin: signalStrength.from
    property alias receivedSignalStrengthMax: signalStrength.to
    property alias receivedSignalStrength: signalStrength.value
    property alias receivedSignalStrengthUnit: signalStrength.unit

    property alias signalToNoiseRatioMin: signalToNoiseRatio.from
    property alias signalToNoiseRatioMax: signalToNoiseRatio.to
    property alias signalToNoiseRatio: signalToNoiseRatio.value
    property alias signalToNoiseRatioUnit: signalToNoiseRatio.unit

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

        from: 0.0
        to: 127.0

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
        id: signalToNoiseRatio

        from: 0.0
        to: 127.0

        unit: 'dB'
    }
}
