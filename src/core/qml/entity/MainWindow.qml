import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import CCStartup

AppStartupItem {
    id: root
    asynchronous: true

    AppStartupComponent {
        id: mainPlaneComp

        Rectangle {
            width: 200
            height: 200
            anchors.centerIn: parent
            color: "gray"
        }
    }
}
