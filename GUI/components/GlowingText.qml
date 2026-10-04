import QtQml
import QtQuick
import QtQuick.Effects

Item {
    property alias text: textContent.text
    property alias font: textContent.font
    property alias color: textContent.color

    property alias brightness: effect.brightness
    property alias contrast: effect.contrast
    property alias blur: effect.blur

    width: textContent.width
    height: textContent.height

    Text {
        id: textContent

        visible: false // Not shown; used as a template for the glow effect

        color: "#A8D6F2"
    }

    MultiEffect {
        id: effect

        anchors.fill: parent

        source: textContent

        // Bloom effect
        shadowEnabled: true
        shadowColor: textContent.color
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
