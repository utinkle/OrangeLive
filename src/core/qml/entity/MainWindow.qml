import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import CCStartup
import OL.Core

AppStartupItem {
    id: root
    asynchronous: true

    AppStartupComponent {
        id: mainContentComp

        MainContent {
            id: mainContent
            anchors.fill: parent
            // transform: Rotation {
            //     origin.x: parent.width
            //     origin.y: 0
            //     axis { x: 0; y: 1; z: 0 }
            //     angle: -switcher.outAnimation * 90
            // }

            // transform: Rotation {
            //     origin.x: 0
            //     origin.y: 0
            //     axis { x: 0; y: 1; z: 0 }
            //     angle: switcher.inAnimation * 90
            // }
        }
    }

    AppStartupComponent {
        depends: mainContentComp

        Rectangle {
            width: 200
            height: 200
            anchors.centerIn: parent
            color: "gray"
        }

        Component.onCompleted: {
            OLModuleLoader.moduleTemplate = Qt.createComponent("TemplateComponent.qml",
                                                               AppStartupItem.mainContent)
        }
    }
}
