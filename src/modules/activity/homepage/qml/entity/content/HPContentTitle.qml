import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Effects
import QtQuick.Controls.Material

import OL.Core

Item {
    id: root

    Rectangle {
        anchors.fill: parent
        color: "#1C1C1E"
        radius: 12

        Row {
            id: hpTitleRow
            anchors.left: parent.left
            anchors.leftMargin: 16
            anchors.verticalCenter: parent.verticalCenter
            spacing: 12

            Rectangle {
                width: 32
                height: 32
                radius: 8
                color: "#0A84FF"

                Image {
                    anchors.centerIn: parent
                    width: 20
                    height: 20
                    source: "qrc:/OLHomePage/resources/images/OrangeLive.png"
                }
            }

            Text {
                anchors.verticalCenter: parent.verticalCenter
                font.bold: true
                font.pixelSize: 18
                font.weight: Font.Bold
                text: "橙子播放器"
                color: "#FFFFFF"
            }

            Rectangle {
                width: 1
                height: 24
                color: "#3A3A3C"
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        OLLogoItem {
            anchors.left: hpTitleRow.right
            anchors.leftMargin: 16
            anchors.verticalCenter: parent.verticalCenter
            scale: 0.9
        }
    }
}