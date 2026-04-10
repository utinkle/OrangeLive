import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material

import OL.Control

Rectangle {
    id: root

    default property alias contentData: contentItem.data
    color: "#0A0A0A"

    ColumnLayout {
        anchors.fill: parent
        spacing: 12

        Item {
            Layout.preferredHeight: Style.statusBarHeight
            Layout.fillWidth: true
        }

        HPContentTitle {
            Layout.preferredHeight: 56
            Layout.fillWidth: true
            Layout.leftMargin: 16
            Layout.rightMargin: 16
        }

        Item {
            id: contentItem
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.topMargin: 4
            Layout.bottomMargin: 4
        }
    }
}