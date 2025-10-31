import QtQuick
import QtQuick.Controls

import OL.Core

OLModuleTemplate {
    id: root

    readonly property int listviewLayout: SwipeView.view ? SwipeView.view.layoutState : ModuleView.ModuleLayoutState.InFullscreen

    Item {
        id: surfaceItem
        anchors.fill: parent

        Rectangle {
            anchors.fill: parent
            color: "green"
        }
    }

    surface: surfaceItem

    TapHandler {
        id: tapHandler

        acceptedDevices: PointerDevice.Mouse | PointerDevice.TouchPad | PointerDevice.TouchScreen
        longPressThreshold: 2
        gesturePolicy: TapHandler.WithinBounds
        onLongPressed: {
            root.SwipeView.view.layoutState = ModuleView.ModuleLayoutState.InLayout
        }
    }

    states: [
        State {
            name: "IN_LAYOUT"
            when: root.listviewLayout === ModuleView.ModuleLayoutState.InLayout
            PropertyChanges {
                target: root
                scale: 0.7
            }
        },
        State {
            name: "InFullscreen"
            when: root.listviewLayout  === ModuleView.ModuleLayoutState.InFullscreen
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
            duration: 200
        }
    }

    Component.onCompleted: {
        if (information.name === "HomePage")
            root.SwipeView.view.currentIndex = index
    }
}
