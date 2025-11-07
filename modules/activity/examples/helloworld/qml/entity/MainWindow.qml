import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material

import CCStartup

AppStartupItem {
    id: root
    asynchronous: true

    AppStartupComponent {
        id: mainContentComp

        Rectangle {
            id: rect1
            anchors.fill: parent
            color: "darkcyan"

            Text {
                anchors.centerIn: parent

                text: "这是一个示例代码"
                font.bold: true
                color: Material.primaryColor
            }
        }
    }
}
