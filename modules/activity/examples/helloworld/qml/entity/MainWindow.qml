import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material

import CCStartup
import OL.Core
import OL.Control

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

            Connections {
                target: rect1.OLModulePage

                function onIsActiveChanged() {
                    if (rect1.OLModulePage.isActive) {
                        Style.mobile.statusbarTheme = Style.Dark
                    }
                }
            }
        }
    }

    onPopulateChanged: {
        if (AppStartupItem.rect1.OLModulePage.isActive) {
            Style.mobile.statusbarTheme = Style.Light
        }
    }
}
