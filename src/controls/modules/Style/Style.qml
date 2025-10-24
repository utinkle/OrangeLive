pragma Singleton

import QtQuick
import MobileUI

QtObject {
    readonly property MobileUI mobile: MobileUI {}

    readonly property color backgroundColor: "white"
    readonly property color backgroundTitleColor: "#F0F0F0"

    readonly property color borderColor: Qt.rgba(0.75, 0.75, 0.75, 1)
    readonly property int borderWidth: 1
    readonly property int focusPadding: 1

    readonly property int statusBarHeight: mobile.statusbarHeight
    readonly property int navigationBarHeight: mobile.navbarHeight
    readonly property int footerBarHeight: 100

    readonly property int maxIconSize: 32
    readonly property int mediumIconSize: 24
    readonly property int smallIconSize: 16

    readonly property int animationDuration: 300

    readonly property int maxSpacing: 40
    readonly property int mediumSpacing: 20
    readonly property int smallSpacing: 10

    readonly property QtObject fontSize: QtObject {
        readonly property int t1: 16
        readonly property int t2: 12
        readonly property int t3: 10
    }

    readonly property SearchEditStyle searchEdit: SearchEditStyle {}
    readonly property CustomButtonStyle customButton: CustomButtonStyle {}
}
