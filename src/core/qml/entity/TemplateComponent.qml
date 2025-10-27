import QtQuick

import OL.Core

OLModuleTemplate {
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
}
