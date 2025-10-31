import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material

import OL.Core
import OL.Control

Item {
    id: root

    enum ModuleLayoutState {
        InFullscreen,
        InLayout
    }

    Rectangle {
        id: background
        anchors.fill: parent
        color: Qt.rgba(0.3, 0.3, 0.3, 1.0)
    }

    SwipeView {
        id: contentView

        anchors.fill: parent
        property int layoutState: ModuleView.ModuleLayoutState.InFullscreen
        property real rotateAngle: 0

        contentData: OLModuleLoader.runingModuleItems
        orientation: Qt.Horizontal
        interactive: layoutState === ModuleView.ModuleLayoutState.InLayout
    }

    PageIndicator {
        id: indicator

        visible: contentView.layoutState === ModuleView.ModuleLayoutState.InLayout
        count: contentView.count
        currentIndex: contentView.currentIndex

        anchors.bottom: contentView.bottom
        anchors.bottomMargin: Style.navigationBarHeight + 20
        anchors.horizontalCenter: parent.horizontalCenter

        delegate: Rectangle {
            implicitWidth: 60
            implicitHeight: 4

            radius: implicitHeight / 2
            color: Qt.rgba(0.65, 0.65, 0.65)

            opacity: index === indicator.currentIndex ? 0.95 : indicator.pressed ? 0.7 : 0.45
            required property int index
            Behavior on opacity {
                OpacityAnimator {
                    duration: 100
                }
            }
        }
    }
}
