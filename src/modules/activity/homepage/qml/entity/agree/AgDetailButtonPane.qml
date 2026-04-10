import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material

import OL.Control

Item {
    id: root
    signal agreeButtonClicked
    signal backButtonClicked

    CustomButton {
        anchors.left: parent.left
        anchors.leftMargin: 20

        font.pixelSize: 12
        Material.roundedScale: Material.SmallScale
        icon.name: "material:arrow_back_ios_new"
        icon.width: 16
        icon.height: 16

        onClicked: root.backButtonClicked()
    }

    Button {
        anchors.centerIn: parent
        text: "我已阅读并理解"
        font.pixelSize: 12
        Material.roundedScale: Material.SmallScale
        Material.foreground: Material.primary
        Material.background: Material.accent

        onClicked: root.agreeButtonClicked()
    }
}
