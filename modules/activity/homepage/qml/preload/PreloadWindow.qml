import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Effects

import CCStartup

AppPreloadItem {
    id: root

    preloadSurface: AppPreloadItem.ItemSurface
    loadingOverlay: Rectangle {
        color: "blue"

        Text {
            anchors.centerIn: parent
            text: "这是一个测试预览"
        }
    }

    autoExitOverlay: true
    transitionGroup:  ScaleTransition {}
}
