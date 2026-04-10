import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    color: "transparent"
    property var playlistManager: null

    // 收藏的频道详情（需要从所有源的频道中查找）
    property var favoriteDetails: []

    function updateFavoriteDetails() {
        var details = []
        if (!playlistManager) return
        var favUrls = playlistManager.favorites
        for (var i = 0; i < favUrls.length; ++i) {
            var url = favUrls[i]
            // 遍历所有源的频道查找
            for (var s = 0; s < playlistManager.sources.length; ++s) {
                var src = playlistManager.sources[s]
                for (var c = 0; c < src.channels.count; ++c) {
                    var ch = src.channels.get(c)
                    if (ch.url === url) {
                        details.push({
                            url: url,
                            title: ch.title,
                            group: ch.group,
                            isLive: ch.isLive
                        })
                        break
                    }
                }
            }
            // 如果没找到，也保留 url 作为标题
            if (details.length < i+1) {
                details.push({ url: url, title: url, group: "未知", isLive: false })
            }
        }
        favoriteDetails = details
    }

    Component.onCompleted: updateFavoriteDetails()
    Connections {
        target: playlistManager
        function onFavoritesChanged() { updateFavoriteDetails() }
        function onSourcesChanged() { updateFavoriteDetails() }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 12

        ListView {
            id: favList
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 8
            clip: true
            model: favoriteDetails

            delegate: Rectangle {
                required property var modelData
                width: favList.width
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
                            text: modelData.title
                            color: "#FFFFFF"
                            font.pixelSize: 14
                            font.weight: Font.Medium
                            elide: Text.ElideRight
                            Layout.fillWidth: true
                        }
                        Text {
                            text: modelData.group
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
                            // TODO: 打开播放器
                            console.log("播放收藏:", modelData.url)
                        }
                    }

                    Button {
                        text: "取消收藏"
                        background: Rectangle {
                            color: "transparent"
                        }
                        contentItem: Text {
                            text: "☆"
                            color: "#FFD700"
                            font.pixelSize: 16
                        }
                        onClicked: {
                            if (playlistManager) playlistManager.removeFavorite(modelData.url)
                        }
                    }
                }
            }

            Label {
                anchors.centerIn: parent
                visible: favList.count === 0
                text: "暂无收藏"
                color: "#8E8E93"
            }
        }
    }
}