import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    color: "#0A0A0A"
    objectName: "profilePage"

    property var playlistManager: null

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 16

        // 顶部 TabBar 切换三个子页面
        TabBar {
            id: profileTabBar
            Layout.fillWidth: true
            Layout.preferredHeight: 48
            background: Rectangle {
                color: "#1C1C1E"
                radius: 12
            }

            TabButton {
                text: "浏览记录"
                width: parent.width / 3
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
            }
            TabButton {
                text: "收藏列表"
                width: parent.width / 3
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
            }
            TabButton {
                text: "播放列表管理"
                width: parent.width / 3
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
            }
        }

        // 内容区域
        StackLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: profileTabBar.currentIndex

            // 浏览记录
            HistoryPage {
                playlistManager: root.playlistManager
            }

            // 收藏列表
            FavoritesPage {
                playlistManager: root.playlistManager
            }

            // 播放列表管理
            SourcesManagerPage {
                playlistManager: root.playlistManager
            }
        }
    }
}