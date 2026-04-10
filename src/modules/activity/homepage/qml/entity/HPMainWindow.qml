import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Basic

import CCStartup
import OL.Core
import OL.Control
import OL.HomePage

import "agree"
import "content"

AppStartupItem {
    id: root
    asynchronous: true

    // 全局配置（后续扩展：主题、默认源等）
    Config {
        id: config
        name: "homepage"
        group: "default"

        property ConfigObject system: ConfigObject {
            property bool agreementAccepted: false
        }

        property ConfigObject session: ConfigObject {
        }

        property ConfigObject ui: ConfigObject {
            property string theme: "dark"   // "dark" or "light"
        }
    }

    readonly property var playlistManager: PlaylistManager { }

    // 监听应用后台，暂停所有播放器（后续实现）
    Connections {
        target: Qt.application
        function onActiveChanged() {
            if (!Qt.application.active) {
                // 后续实现：暂停全局播放器
                // if (root.globalPlayer) root.globalPlayer.pausePlayback()
            }
        }
    }

    AppStartupComponent {
        id: mainContentComp

        HPMainContent {
            id: content
            anchors.fill: parent
            property alias playlistManager: root.playlistManager

            ColumnLayout {
                anchors.fill: parent
                spacing: 0

                HPContentTitle {
                    Layout.preferredHeight: 56
                    Layout.fillWidth: true
                    Layout.leftMargin: 16
                    Layout.rightMargin: 16
                    Layout.topMargin: Style.statusBarHeight
                }

                StackView {
                    id: mainStack
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    initialItem: homePageComp

                    Component {
                        id: homePageComp
                        HomePage { }
                    }

                    Component {
                        id: profilePageComp
                        ProfilePage { }
                    }

                    Component.onCompleted: {
                        push(homePageComp)
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 56 + Style.navigationBarHeight
                    color: "#1C1C1E"

                    TabBar {
                        id: bottomTabBar
                        width: parent.width
                        height: 56
                        anchors.bottom: parent.top
                        anchors.bottomMargin: 0

                        background: Rectangle {
                            color: "transparent"
                        }

                        TabButton {
                            text: "首页"
                            width: parent.width / 2
                            contentItem: Text {
                                text: parent.text
                                color: parent.checked ? "#0A84FF" : "#8E8E93"
                                font.pixelSize: 14
                                font.weight: parent.checked ? Font.Medium : Font.Normal
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                            background: Rectangle {
                                color: "transparent"
                                Rectangle {
                                    width: parent.width * 0.6
                                    height: 2
                                    color: "#0A84FF"
                                    anchors.bottom: parent.bottom
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    visible: parent.checked
                                }
                            }
                            onClicked: {
                                if (mainStack.currentItem.objectName !== "homePage")
                                    mainStack.replace(homePageComp)
                            }
                        }

                        TabButton {
                            text: "我的"
                            width: parent.width / 2
                            contentItem: Text {
                                text: parent.text
                                color: parent.checked ? "#0A84FF" : "#8E8E93"
                                font.pixelSize: 14
                                font.weight: parent.checked ? Font.Medium : Font.Normal
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                            background: Rectangle {
                                color: "transparent"
                                Rectangle {
                                    width: parent.width * 0.6
                                    height: 2
                                    color: "#0A84FF"
                                    anchors.bottom: parent.bottom
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    visible: parent.checked
                                }
                            }
                            onClicked: {
                                if (mainStack.currentItem.objectName !== "profilePage")
                                    mainStack.replace(profilePageComp)
                            }
                        }
                    }
                }
            }
        }
    }

    onPopulateChanged: {
        if (AppStartupItem.content.OLModulePage.isActive) {
            Style.mobile.statusbarTheme = Style.Dark
        }
    }
}