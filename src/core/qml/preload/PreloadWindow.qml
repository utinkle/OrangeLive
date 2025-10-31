import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Effects

import CCStartup
import OL.Control

AppPreloadItem {
    id: root

    loadingOverlay: PreloadOverlay {
        id: overlay

        Button {
            id: skipButton
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.topMargin: 10 + Style.statusBarHeight
            anchors.rightMargin: 10

            Material.background: Qt.color("#CCE04F5F")
            Material.foreground: Qt.color("#FFFFFFFF")
            Material.roundedScale: Material.ExtraLargeScale
            Material.elevation: 4

            visible: preloadCountdown.running
            text: qsTr("Skip") + " " + preloadCountdown.current

            onClicked: {
                preloadCountdown.running = false
            }
        }
    }

    autoExitOverlay: true
    overlayExitWhen: !preloadCountdown.running
    transitionGroup:  FlickTransition {}
    initialProperties: InitialProperties {
        readonly property bool visible: true
        readonly property color color: "white"
    }

    Countdown {
        id: preloadCountdown
        interval: 10
        running: root.visible
    }
}
