import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material

import OrangeLive

TextField {
    id: control

    opacity: enabled ? 1 : 0.4
    leftPadding: (editting) ? searchIcon.width + 2 * Style.searchEdit.iconMargin : 0
    selectionColor: control.palette.highlight
    selectedTextColor: control.palette.highlightedText
    verticalAlignment: TextInput.AlignVCenter

    background: Rectangle {
        radius: Style.searchEdit.radius
        color: Style.searchEdit.backgroundColor

        Loader {
            anchors.fill: parent
            active: control.activeFocus
            sourceComponent: Rectangle {
                anchors {
                    fill: parent
                    margins: -Style.focusPadding
                }

                color: "transparent"
                radius: Style.searchEdit.radius
                border { width: 1; color: control.palette.highlight }
            }
        }
    }
}
