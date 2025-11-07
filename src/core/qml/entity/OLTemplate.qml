import QtQuick
import QtQuick.Controls

import OL.Core

OLModuleTemplate {
    id: root

    readonly property real baseRatate: (itemIndex - SwipeView.view.currentIndex) * 45

    property real progress: ((x - SwipeView.view.contentX) / SwipeView.view.width) % 0.5
    property real rotationOriginX: 0
    property real rotationAngle: baseRatate + progress * 90
    property int itemIndex: root.SwipeView.index

    Item {
        id: surfaceItem
        anchors.fill: parent

        Rectangle {
            anchors.fill: parent
            color: "green"
        }
    }

    surface: surfaceItem

    Loader {
        active: root.SwipeView.view.layoutState === ModuleView.ModuleLayoutState.InLayout
        anchors.fill: parent

        sourceComponent: ShaderEffectSource {
            id: surfaceSource
            hideSource: true
            live: false
            sourceItem: surfaceItem

            Connections {
                target: root.Screen

                function onPrimaryOrientationChanged() {
                    surfaceSource.scheduleUpdate()
                }
            }
        }
    }

    TapHandler {
        enabled: root.SwipeView.view.layoutState !== ModuleView.ModuleLayoutState.InLayout
        acceptedDevices: PointerDevice.Mouse | PointerDevice.TouchPad | PointerDevice.TouchScreen
        longPressThreshold: 2
        gesturePolicy: TapHandler.WithinBounds
        onLongPressed: {
            root.SwipeView.view.layoutState = ModuleView.ModuleLayoutState.InLayout
        }
    }

    TapHandler {
        enabled: root.SwipeView.view.layoutState === ModuleView.ModuleLayoutState.InLayout
        acceptedDevices: PointerDevice.Mouse | PointerDevice.TouchPad | PointerDevice.TouchScreen
        gesturePolicy: TapHandler.WithinBounds
        onTapped: {
            if (!root.SwipeView.isCurrentItem || (progress > 0 && progress < 1))
                return

            root.SwipeView.view.layoutState = ModuleView.ModuleLayoutState.InFullscreen
        }
    }

    Component.onCompleted: {
        updateDirection()
    }

    function updateDirection()
    {
        if (root.SwipeView.view.movingDirection === 1) {
            if ((itemIndex === root.SwipeView.view.currentIndex)
                    || (itemIndex === root.SwipeView.view.currentIndex - 1)) {
                rotationOriginX = width
            } else if ((itemIndex === root.SwipeView.view.currentIndex + 1)) {
                rotationOriginX = 0
            }
        } else if ((root.SwipeView.view.movingDirection === -1)) {
            if ((itemIndex === root.SwipeView.view.currentIndex)
                    || (itemIndex === root.SwipeView.view.currentIndex + 1)) {
                rotationOriginX = 0
            } else if ((itemIndex === root.SwipeView.view.currentIndex - 1)) {
                rotationOriginX = width
            }
        }
    }

    Connections {
        target: root.SwipeView.view

        function onDirectStateChanged() {
            updateDirection()
        }
    }

    transform: Rotation {
        origin.x: root.rotationOriginX
        origin.y: root.height / 2
        axis { x: 0; y: 1; z: 0 }
        angle: root.rotationAngle
    }
}
