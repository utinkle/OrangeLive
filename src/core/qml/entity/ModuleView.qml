import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material

import OL.Core

Item {
    id: root

    Rectangle {
        anchors.fill: parent
        color: Qt.rgba(0.3, 0.3, 0.3, 1.0)
    }

    enum ModuleLayoutState {
        InLayout,
        InFullscreen
    }

    ListView {
        anchors.fill: parent
        property int layoutState: ModuleView.ModuleLayoutState.InLayout

        model: OLModuleLoader.runingModuleItems
        orientation: Qt.Horizontal
        snapMode: ListView.SnapOneItem
    }
}
