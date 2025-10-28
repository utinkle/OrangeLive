import QtQuick

import OL.Core

OLModuleTemplate {
    id: root

    readonly property real baseRatate: !visible ? 0 : (index - ListView.view.currentIndex) * 90
    property real rotateAngle: 0

    function isNextItem() {
        var nextIndex = root.ListView.view.currentIndex + 1
        return nextIndex >= root.ListView.view.count ? false : (nextIndex === index)
    }

    function isPreviousItem() {
        var prevIndex = root.ListView.view.currentIndex - 1
        return prevIndex < 0 ? false : (prevIndex === index)
    }

    visible: isNextItem() || isPreviousItem() || ListView.isCurrentItem
    anchors.fill: parent

    surface: Item {
        anchors.fill: parent

        Rectangle {
            width: 200
            height: 200
            anchors.centerIn: parent
            color: "green"
        }
    }

    transform: Rotation {
        origin.x: width
        origin.y: 0
        axis { x: 0; y: 1; z: 0 }
        angle: baseRatate - rotateAngle
    }

    TapHandler {
        id: tapHandler

        acceptedDevices: PointerDevice.Mouse | PointerDevice.TouchPad | PointerDevice.TouchScreen
        longPressThreshold: 3
        gesturePolicy: TapHandler.WithinBounds
        onLongPressed: {
            console.debug("loog pressed")
            root.ListView.view.layoutState = ModuleView.ModuleLayoutState.InLayout
        }
    }

    states: [
        State {
            name: "IN_LAYOUT"
            PropertyChanges {
                target: root
                scale: 0.7
            }
        },
        State {
            name: "InFullscreen"
            PropertyChanges {
                target: root
                scale: 1.0
            }
        }
    ]

    transitions: Transition {
        PropertyAnimation {
            properties: "scale"
            easing.type: Easing.InCubic
        }
    }
}
