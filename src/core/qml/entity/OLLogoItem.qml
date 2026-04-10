import QtQuick
import QtQuick.Effects
import QtQuick.Controls.Material

Item {
    width: appTitleText.width + 10
    height: appTitleText.height + 10

    Rectangle {
        id: appTitleBackgroundSource
        anchors.fill: parent
        radius: 6
        visible: false
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#FFF5F0" }
            GradientStop { position: 0.5; color: "#FFE8D9" }
            GradientStop { position: 1.0; color: "#FFD8C8" }
        }
    }

    MultiEffect {
        source: appTitleBackgroundSource
        anchors.fill: appTitleBackgroundSource
        shadowEnabled: true
        shadowColor: Qt.rgba(0, 0, 0, 0.6)
        shadowOpacity: 0.5
        shadowBlur: 0.4
    }

    Text {
        id: appTitleText
        anchors.centerIn: parent

        font.bold: true
        font.pixelSize: 8
        text: "橙子工具台"
        color: Qt.lighter(Material.accentColor, 1.1)
    }
}
