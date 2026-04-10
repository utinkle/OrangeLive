import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    color: "#0A0A0A"
    objectName: "homePage"

    // 外部传入的 playlistManager
    property var playlistManager: null

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 16

        // 源选择行
        RowLayout {
            Layout.fillWidth: true
            spacing: 12

            Label {
                text: "播放列表:"
                color: "#FFFFFF"
                font.pixelSize: 14
            }

            ComboBox {
                id: sourceCombo
                Layout.fillWidth: true
                model: playlistManager ? playlistManager.sources.map(s => s.name) : []
                currentIndex: playlistManager ? playlistManager.currentSourceIndex : -1
                onCurrentIndexChanged: {
                    if (playlistManager && currentIndex >= 0)
                        playlistManager.setCurrentSource(currentIndex)
                }
                background: Rectangle {
                    color: "#2C2C2E"
                    radius: 8
                    border.color: "#3A3A3C"
                    border.width: 0.5
                }
                contentItem: Text {
                    text: sourceCombo.currentText
                    color: "#FFFFFF"
                    font.pixelSize: 13
                    verticalAlignment: Text.AlignVCenter
                    leftPadding: 12
                    rightPadding: 24
                }
                indicator: Image {
                    source: "qrc:/OLHomePage/resources/images/dropdown_arrow.png"
                    width: 16
                    height: 16
                    anchors.right: parent.right
                    anchors.rightMargin: 8
                    anchors.verticalCenter: parent.verticalCenter
                }
            }

            Button {
                text: "添加源"
                background: Rectangle {
                    color: "#0A84FF"
                    radius: 8
                }
                contentItem: Text {
                    text: "添加源"
                    color: "#FFFFFF"
                    font.pixelSize: 13
                }
                onClicked: {
                    // TODO: 弹出对话框添加源
                    console.log("添加源")
                }
            }
        }

        // 分组过滤行
        RowLayout {
            Layout.fillWidth: true
            spacing: 12

            Label {
                text: "分组:"
                color: "#FFFFFF"
            }
            ComboBox {
                id: groupCombo
                Layout.fillWidth: true
                model: playlistManager ? ["全部"].concat(playlistManager.getGroups()) : ["全部"]
                currentIndex: 0
                onCurrentTextChanged: {
                    if (playlistManager) {
                        playlistManager.currentGroup = currentText === "全部" ? "" : currentText
                        playlistManager.applyFilter()
                    }
                }
                background: Rectangle {
                    color: "#2C2C2E"
                    radius: 8
                    border.color: "#3A3A3C"
                    border.width: 0.5
                }
                contentItem: Text {
                    text: groupCombo.currentText
                    color: "#FFFFFF"
                    font.pixelSize: 13
                    verticalAlignment: Text.AlignVCenter
                    leftPadding: 12
                    rightPadding: 24
                }
                indicator: Image {
                    source: "qrc:/OLHomePage/resources/images/dropdown_arrow.png"
                    width: 16
                    height: 16
                    anchors.right: parent.right
                    anchors.rightMargin: 8
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
        }

        // 搜索框
        TextField {
            Layout.fillWidth: true
            placeholderText: "搜索频道"
            onTextChanged: {
                if (playlistManager) {
                    playlistManager.searchKeyword = text
                    playlistManager.applyFilter()
                }
            }
            background: Rectangle {
                color: "#2C2C2E"
                radius: 8
                border.color: "#3A3A3C"
                border.width: 0.5
            }
            color: "#FFFFFF"
            placeholderTextColor: "#8E8E93"
            font.pixelSize: 13
            leftPadding: 36
            rightPadding: 12
            Image {
                source: "qrc:/OLHomePage/resources/images/search_icon.png"
                width: 16
                height: 16
                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.verticalCenter: parent.verticalCenter
                opacity: 0.6
            }
        }

        // 加载指示器
        BusyIndicator {
            Layout.alignment: Qt.AlignHCenter
            running: playlistManager ? playlistManager.loading : false
            visible: running
        }

        // 错误提示
        Text {
            visible: playlistManager && playlistManager.errorString !== ""
            text: playlistManager ? playlistManager.errorString : ""
            color: "#FF3B30"
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }

        // 频道列表
        ListView {
            id: channelListView
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 8
            clip: true
            model: playlistManager ? playlistManager.filteredChannelsModel : null

            delegate: Rectangle {
                required property var model
                width: channelListView.width
                height: 72
                radius: 10
                color: channelMouse.containsMouse ? "#3A3A3C" : "#2C2C2E"
                border.color: model.isLive ? "#FF3B30" : "#0A84FF"
                border.width: model.isLive ? 1.5 : 0.5

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 10

                    Rectangle {
                        width: 44
                        height: 44
                        radius: 22
                        color: model.isLive ? "#FF3B3020" : "#0A84FF20"
                        Text {
                            anchors.centerIn: parent
                            text: (model.title || "?").slice(0, 1).toUpperCase()
                            color: model.isLive ? "#FF3B30" : "#0A84FF"
                            font.pixelSize: 18
                            font.weight: Font.Bold
                        }
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 4
                        Text {
                            text: model.title
                            color: "#FFFFFF"
                            elide: Text.ElideRight
                            font.pixelSize: 14
                            font.weight: Font.Medium
                        }
                        Text {
                            text: model.group || "未分组"
                            color: "#8E8E93"
                            font.pixelSize: 11
                        }
                    }

                    Rectangle {
                        visible: model.isLive
                        width: 40
                        height: 20
                        radius: 10
                        color: "#FF3B30"
                        Text {
                            anchors.centerIn: parent
                            text: "LIVE"
                            color: "#FFFFFF"
                            font.pixelSize: 10
                            font.weight: Font.Bold
                        }
                    }

                    Button {
                        text: playlistManager ? (playlistManager.isFavorite(model.url) ? "★" : "☆") : "☆"
                        font.pixelSize: 18
                        background: Rectangle { color: "transparent" }
                        contentItem: Text {
                            text: parent.text
                            color: playlistManager && playlistManager.isFavorite(model.url) ? "#FFD700" : "#8E8E93"
                            font.pixelSize: 18
                        }
                        onClicked: {
                            if (playlistManager) playlistManager.toggleFavorite(model.url)
                        }
                    }
                }

                HoverHandler { id: channelMouse }
                TapHandler {
                    onTapped: {
                        // 播放频道
                        console.log("播放:", model.url)
                        // TODO: 打开播放器页面
                        // 先添加到历史记录
                        if (playlistManager) playlistManager.addHistory(model)
                    }
                }
            }

            // 空状态提示
            Label {
                anchors.centerIn: parent
                visible: channelListView.count === 0 && !(playlistManager && playlistManager.loading)
                text: "暂无频道，请添加播放列表源或检查网络"
                color: "#8E8E93"
                wrapMode: Text.WordWrap
                width: parent.width - 40
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }

    // 当 playlistManager 改变时，刷新源和分组
    Connections {
        target: playlistManager
        function onCurrentSourceIndexChanged() {
            sourceCombo.currentIndex = playlistManager.currentSourceIndex
            // 刷新分组
            var groups = playlistManager.getGroups()
            groupCombo.model = ["全部"].concat(groups)
        }
        function onFilteredChannelsModelChanged() {
            // 列表自动刷新
        }
    }
}