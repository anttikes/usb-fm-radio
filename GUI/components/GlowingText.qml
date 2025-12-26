import QtQml
import QtQuick
import QtQuick.Effects

Item {
    property alias text: text.text
    property alias font: text.font
    property alias color: text.color

    property alias brightness: effect.brightness
    property alias contrast: effect.contrast
    property alias blur: effect.blur

    width: text.width
    height: text.height

    Text {
        id: text

        visible: false // Not shown; used as a template for the glow effect

        color: "#A8D6F2"
    }

    MultiEffect {
        id: effect

        anchors.fill: parent

        source: text

        // Bloom effect
        shadowEnabled: true
        shadowColor: text.color
        shadowBlur: 0.8
        shadowHorizontalOffset: 0
        shadowVerticalOffset: 0

        // Intensity
        brightness: 0.3
        contrast: 0.2

        // Adding a slight blur to the text itself makes it
        // look less like "pixels" and more like "light"
        blurEnabled: true
        blur: 0.3
    }
}
