import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import MobileUI
import CCStartup
import OL.Core
import OL.Control

AppStartupItem {
    id: root
    asynchronous: true

    AppStartupComponent {
        id: mainContentComp

        MainContent {
            id: mainContent
            anchors.fill: parent

            ModuleView {
                id: moduleView
                anchors.fill: parent
            }

            Component.onCompleted: {
                OLModuleLoader.moduleTemplate = Qt.createComponent("TemplateComponent.qml")
            }
        }
    }

    onPopulateChanged: {
        // TODO: 更精确控制
        Style.mobile.statusbarTheme = MobileUI.Dark
    }
}
