import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root

    radius: 8
    color: Qt.rgba(0.0, 0.0, 0.0, 0.05)

    Row {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 6

        Image {
            anchors.verticalCenter: parent.verticalCenter
            source: "qrc:/OLHomePage/resources/images/OrangeLive.png"
            sourceSize: Qt.size(24, 24)
        }

        Text {
            anchors.verticalCenter: parent.verticalCenter
            font.bold: true
            font.pixelSize: 14
            text: "橙子工具台"
        }
    }
}
