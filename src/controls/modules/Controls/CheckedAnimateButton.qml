import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material

import OrangeLive

CustomButton {
    id: root

    autoExclusive: true
    checkable: true
    display: Button.TextUnderIcon

    Material.background: checked ? "white" : "transparent"
    Material.elevation: checked ? 4 : 0

    state: checked ? "CHECKED_STATE" : "UNCHECKED_STATE"
    states: [
        State {
            name: "CHECKED_STATE"
            when: checked

            PropertyChanges {
                target: root
                icon.width: Style.mediumIconSize
                icon.height: Style.mediumIconSize
                font.pointSize: Style.fontSize.t2
                font.bold: true
            }
        },
        State {
            name: "UNCHECKED_STATE"
            when: !checked

            PropertyChanges {
                target: root
                icon.width: Style.smallIconSize
                icon.height: Style.smallIconSize
                font.pointSize: Style.fontSize.t3
                font.bold: false
            }
        }
    ]

    transitions: Transition {
        PropertyAnimation {
            properties: "icon.width,icon.height,font.pointSize"

            duration: Style.animationDuration
            easing.type: Easing.InOutBack
        }
    }
}
