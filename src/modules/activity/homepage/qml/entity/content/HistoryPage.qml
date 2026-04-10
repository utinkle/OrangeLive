import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    color: "transparent"
    property var playlistManager: null

    ColumnLayout {
        anchors.fill: parent
        spacing: 12

        RowLayout {
            Layout.fillWidth: true
            Item { Layout.fillWidth: true }
            Button {
                text: "清空全部"
                background: Rectangle {
                    color: "#FF3B30"
                    radius: 8
                }
                contentItem: Text {
                    text: "清空全部"
                    color: "#FFFFFF"
                    font.pixelSize: 12
                }
                onClicked: {
                    if (playlistManager) {
                        playlistManager.history = []
                        playlistManager.saveSettings()
                    }
                }
            }
        }

        ListView {
            id: historyList
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 8
            clip: true
            model: playlistManager ? playlistManager.history : []

            delegate: Rectangle {
                required property var modelData
                width: historyList.width
                height: 60
                radius: 8
                color: "#2C2C2E"

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 10

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 4
                        Text {
                            text: modelData.title || "未知"
                            color: "#FFFFFF"
                            font.pixelSize: 14
                            font.weight: Font.Medium
                            elide: Text.ElideRight
                            Layout.fillWidth: true
                        }
                        Text {
                            text: modelData.group || "未分组"
                            color: "#8E8E93"
                            font.pixelSize: 11
                        }
                    }

                    Button {
                        text: "播放"
                        background: Rectangle {
                            color: "#0A84FF"
                            radius: 8
                        }
                        contentItem: Text {
                            text: "播放"
                            color: "#FFFFFF"
                            font.pixelSize: 12
                        }
                        onClicked: {
                            // TODO: 打开播放器页面并播放该频道
                            console.log("播放历史:", modelData.url)
                            if (playlistManager) {
                                // 重新添加到历史（会移到顶部）
                                playlistManager.addHistory(modelData)
                            }
                        }
                    }

                    Button {
                        text: "删除"
                        background: Rectangle {
                            color: "transparent"
                        }
                        contentItem: Text {
                            text: "✕"
                            color: "#8E8E93"
                            font.pixelSize: 14
                        }
                        onClicked: {
                            if (playlistManager) {
                                var idx = playlistManager.history.indexOf(modelData)
                                if (idx !== -1) {
                                    playlistManager.history.splice(idx, 1)
                                    playlistManager.saveSettings()
                                }
                            }
                        }
                    }
                }
            }

            // 空状态
            Label {
                anchors.centerIn: parent
                visible: historyList.count === 0
                text: "暂无浏览记录"
                color: "#8E8E93"
            }
        }
    }
}