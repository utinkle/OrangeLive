import QtQuick
import QtQuick.Controls

import OrangeLive

Item {
    id: root
    implicitWidth: icon.implicitWidth
    implicitHeight: icon.implicitHeight

    property string iconName
    property int iconWidth: -1
    property int iconHeight: -1
    property color iconColor

    Image {
        id: icon

        anchors.centerIn: parent
        source: OrangeLive.toTokIconUrl(iconName, iconColor)
        sourceSize: Qt.size(iconWidth, iconHeight)
        width: iconWidth
        height: iconHeight
    }
}
