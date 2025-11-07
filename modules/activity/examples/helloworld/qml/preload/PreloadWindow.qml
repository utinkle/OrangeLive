import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Effects

import CCStartup

AppPreloadItem {
    id: root

    preloadSurface: AppPreloadItem.ItemSurface
    loadingOverlay: Item {
        BusyIndicator {
            anchors.centerIn: parent
            running: true
        }
    }

    autoExitOverlay: true
    transitionGroup:  ScaleTransition {}
}
