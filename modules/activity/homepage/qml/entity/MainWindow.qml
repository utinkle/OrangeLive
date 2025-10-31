import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import CCStartup

AppStartupItem {
    id: root
    asynchronous: true

    AppStartupComponent {
        id: mainContentComp

        Rectangle {
            id: rect1
            anchors.fill: parent
            color: "pink"

            Text {
                anchors.centerIn: parent
                text: "这是一个测试主界面"
            }
        }
    }
}
