import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material

import OrangeLive

LineEdit {
    id: control
    property bool editting: control.activeFocus || (text.length !== 0)
    property alias placeholder: centerIndicatorLabel.text

    state: "NONEDIT"
    states: [
        State {
            name: "EDITTING"
            when: editting
            AnchorChanges {
                target: centerIndicator
                anchors.left: searchBackground.left
                anchors.verticalCenter: searchBackground.verticalCenter
            }
            PropertyChanges {
                target: centerIndicator
                anchors.leftMargin: Style.searchEdit.iconMargin
            }
            PropertyChanges {
                target: centerIndicatorLabel
                color: "transparent"
            }
        },
        State {
            name: "NONEDIT"
            when: !editting
            AnchorChanges {
                target: centerIndicator
                anchors.horizontalCenter: searchBackground.horizontalCenter
                anchors.verticalCenter: searchBackground.verticalCenter
            }
            PropertyChanges {
                target: centerIndicatorLabel
                color: Style.searchEdit.placeholderTextColor
            }
        }
    ]

    transitions: Transition {
        AnchorAnimation {
            duration: Style.animationDuration
            easing.type: Easing.OutCubic
        }
        ColorAnimation {
            duration: Style.animationDuration
            easing.type: Easing.OutCubic
        }
    }

    Item {
        id: searchBackground
        anchors.fill: parent

        RowLayout {
            id: centerIndicator
            spacing: Style.searchEdit.spacing

            // Search Icon
            TokIcon {
                id: searchIcon
                iconName: "search"
                iconWidth: Style.smallIconSize
                iconHeight: Style.smallIconSize
                iconColor: Style.searchEdit.placeholderTextColor
                Layout.alignment: Qt.AlignVCenter
            }

            Text {
                id: centerIndicatorLabel
                text: qsTr("Search")
                font: control.font
                verticalAlignment: Text.AlignVCenter
            }
        }

        MouseArea {
            anchors.fill: parent
            visible: !editting
            onPressed: (mouse)=> {
                control.forceActiveFocus(Qt.MouseFocusReason)
                mouse.accepted = false
            }
        }
    }
}
