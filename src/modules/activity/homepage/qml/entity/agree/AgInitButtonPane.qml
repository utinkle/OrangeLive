import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material

RowLayout {
    id: root
    spacing: 20

    signal quitButtonClicked
    signal agreeButtonClicked
    property alias agreeEnabled: agreeButton.enabled

    Item { Layout.fillWidth: true }

    Button {
        Layout.alignment: Qt.AlignVCenter

        text: "取消并退出"
        font.pixelSize: 12
        Material.roundedScale: Material.SmallScale

        onClicked: {
            root.quitButtonClicked()
        }
    }

    Rectangle {
        color: Qt.rgba(0.2, 0.2, 0.2, 0.6)
        height: parent.height * 0.8
        width: 1
        Layout.alignment: Qt.AlignVCenter
    }

    Button {
        id: agreeButton
        Layout.alignment: Qt.AlignVCenter

        text: "同意并继续"
        font.pixelSize: 12
        Material.foreground: Material.primary
        Material.background: Material.accent
        Material.roundedScale: Material.SmallScale

        onClicked: {
            root.agreeButtonClicked()
        }
    }

    Item { Layout.fillWidth: true }
}
