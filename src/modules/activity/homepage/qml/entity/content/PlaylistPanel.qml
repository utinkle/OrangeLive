import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material

import OL.Control
import OL.HomePage

Item {
    id: root

    required property PlaylistService playlistService
    signal playUrl(string url)
    signal createTabRequested()

    ColumnLayout {
        id: contentLayout
        anchors.fill: parent
        spacing: 16
        anchors.margins: 16
        anchors.bottomMargin: Style.navigationBarHeight

        // 输入区域卡片
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 60
            radius: 12
            color: "#2C2C2E"

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 8

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 8

                    TextField {
                        id: sourceInput
                        Layout.fillWidth: true
                        Layout.preferredHeight: 40
                        placeholderText: "输入 m3u URL 或本地路径"
                        background: Rectangle {
                            color: "#1C1C1E"
                            radius: 8
                            border.color: "#3A3A3C"
                            border.width: 0.5
                        }
                        color: "#FFFFFF"
                        placeholderTextColor: "#8E8E93"
                        font.pixelSize: 13
                        leftPadding: 10
                        rightPadding: 10
                        selectByMouse: true
                    }

                    Button {
                        text: root.playlistService.loading ? "加载中..." : "加载列表"
                        enabled: !root.playlistService.loading
                        background: Rectangle {
                            color: parent.enabled ? "#0A84FF" : "#2C2C2E"
                            radius: 8
                        }
                        contentItem: Text {
                            text: parent.text
                            color: parent.enabled ? "#FFFFFF" : "#8E8E93"
                            font.pixelSize: 13
                            font.weight: Font.Medium
                        }
                        onClicked: root.playlistService.load(sourceInput.text)
                    }

                    Button {
                        text: "新建页"
                        background: Rectangle {
                            color: parent.pressed ? "#3A3A3C" : "#2C2C2E"
                            radius: 8
                            border.color: "#3A3A3C"
                            border.width: 0.5
                        }
                        contentItem: Text {
                            text: parent.text
                            color: "#FFFFFF"
                            font.pixelSize: 13
                        }
                        onClicked: root.createTabRequested()
                    }
                }
            }
        }

        // 分组选择器
        ComboBox {
            id: groupComboBox
            Layout.fillWidth: true
            Layout.preferredHeight: 40
            leftPadding: 16

            model: ["全部频道"].concat(root.playlistService.groups)
            currentIndex: 0
            onCurrentTextChanged: {
                root.playlistService.currentGroup = currentText === "全部频道" ? "" : currentText
            }

            background: Rectangle {
                color: "#2C2C2E"
                radius: 8
                border.color: "#3A3A3C"
                border.width: 0.5
            }
            contentItem: Text {
                text: parent.currentText
                color: "#FFFFFF"
                font.pixelSize: 13
                verticalAlignment: Text.AlignVCenter
                leftPadding: 10
                rightPadding: 20
            }
            indicator: Image {
                source: "qrc:/OLHomePage/resources/images/dropdown_arrow.png"
                width: 16
                height: 16
                anchors.right: parent.right
                anchors.rightMargin: 10
                anchors.verticalCenter: parent.verticalCenter
            }
            popup: Popup {
                y: groupComboBox.editable ? groupComboBox.height - 5 : 0
                width: groupComboBox.width
                height: Math.min(contentItem.implicitHeight + verticalPadding * 2, groupComboBox.Window.height - topMargin - bottomMargin)
                transformOrigin: Item.Top
                topMargin: 12
                bottomMargin: 12
                verticalPadding: 8

                Material.theme: groupComboBox.Material.theme
                Material.accent: groupComboBox.Material.accent
                Material.primary: groupComboBox.Material.primary

                enter: Transition {
                    // grow_fade_in
                    NumberAnimation { property: "scale"; from: 0.9; easing.type: Easing.OutQuint; duration: 220 }
                    NumberAnimation { property: "opacity"; from: 0.0; easing.type: Easing.OutCubic; duration: 150 }
                }

                exit: Transition {
                    // shrink_fade_out
                    NumberAnimation { property: "scale"; to: 0.9; easing.type: Easing.OutQuint; duration: 220 }
                    NumberAnimation { property: "opacity"; to: 0.0; easing.type: Easing.OutCubic; duration: 150 }
                }

                contentItem: ListView {
                    clip: true
                    implicitHeight: Math.min(200, Math.max(contentHeight, 60))
                    model: groupComboBox.delegateModel
                    currentIndex: groupComboBox.highlightedIndex
                    highlightMoveDuration: 0

                    ScrollIndicator.vertical: ScrollIndicator { }
                }

                background: Rectangle {
                    radius: 4
                    color: parent.Material.dialogColor
                }
            }
        }

        // 搜索框
        TextField {
            Layout.fillWidth: true
            Layout.preferredHeight: 40

            placeholderText: "搜索频道（名称/ID）"
            onTextChanged: root.playlistService.searchKeyword = text

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
            rightPadding: 10

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

        // 错误提示
        Rectangle {
            visible: root.playlistService.errorString.length > 0
            Layout.fillWidth: true
            Layout.preferredHeight: 48
            radius: 8
            color: "#FF3B3020"
            border.width: 1
            border.color: "#FF3B30"

            RowLayout {
                anchors.fill: parent
                anchors.margins: 10

                Image {
                    source: "qrc:/OLHomePage/resources/images/error_icon.png"
                    Layout.preferredWidth: 20
                    Layout.preferredHeight: 20
                }

                Text {
                    text: root.playlistService.errorString
                    color: "#FF3B30"
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                    font.pixelSize: 10
                }
            }
        }

        // EPG信息
        Rectangle {
            visible: root.playlistService.epgUrl.length > 0
            Layout.fillWidth: true
            Layout.preferredHeight: 36
            radius: 6
            color: "#0A84FF10"

            RowLayout {
                anchors.fill: parent
                anchors.margins: 8

                Image {
                    source: "qrc:/OLHomePage/resources/images/epg_icon.png"
                    Layout.preferredWidth: 16
                    Layout.preferredHeight: 16
                }

                Text {
                    text: `EPG: ${root.playlistService.epgUrl}`
                    color: "#0A84FF"
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                    font.pixelSize: 11
                }
            }
        }

        // 最近观看区域
        ColumnLayout {
            Layout.fillWidth: true
            visible: root.playlistService.recentChannels.length > 0
            spacing: 10

            Label {
                text: "最近观看"
                color: "#FFFFFF"
                font.pixelSize: 14
                font.weight: Font.Medium
            }

            ListView {
                Layout.fillWidth: true
                Layout.preferredHeight: 100
                orientation: ListView.Horizontal
                spacing: 10
                model: root.playlistService.recentChannels

                delegate: Rectangle {
                    id: recentItem
                    required property var modelData
                    width: 160
                    height: 88
                    radius: 10
                    color: "#2C2C2E"

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 10
                        spacing: 6

                        Text {
                            text: recentItem.modelData.title || recentItem.modelData.url
                            color: "#FFFFFF"
                            elide: Text.ElideRight
                            Layout.fillWidth: true
                            font.pixelSize: 13
                            font.weight: Font.Medium
                        }

                        Text {
                            text: recentItem.modelData.group || "最近播放"
                            color: "#8E8E93"
                            font.pixelSize: 11
                            Layout.fillWidth: true
                        }
                    }

                    TapHandler {
                        onTapped: {
                            root.playUrl(recentItem.modelData.url)
                        }
                    }

                    HoverHandler { id: hoverHandler }
                }
            }
        }

        // 频道列表标题
        RowLayout {
            Layout.fillWidth: true
            Layout.topMargin: 4

            Label {
                text: `频道列表 (${root.playlistService.filteredChannels.length})`
                color: "#FFFFFF"
                font.pixelSize: 14
                font.weight: Font.Medium
                Layout.fillWidth: true
            }

            Label {
                text: "点击播放"
                color: "#8E8E93"
                font.pixelSize: 11
            }
        }

        // 频道列表
        ListView {
            id: channelList
            Layout.fillWidth: true
            Layout.fillHeight: true

            spacing: 8
            clip: true
            model: root.playlistService.filteredChannels

            delegate: Rectangle {
                id: channelItem
                required property var modelData
                width: channelList.width
                height: 72
                radius: 10
                color: channelMouse.containsMouse ? "#3A3A3C" : "#2C2C2E"
                border.color: channelItem.modelData.isLive ? "#FF3B30" : "#0A84FF"
                border.width: channelItem.modelData.isLive ? 1.5 : 0.5

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 10

                    // 频道图标/首字母
                    Rectangle {
                        Layout.preferredWidth: 44
                        Layout.preferredHeight: 44
                        radius: 22
                        color: channelItem.modelData.isLive ? "#FF3B3020" : "#0A84FF20"

                        Text {
                            anchors.centerIn: parent
                            text: (channelItem.modelData.title || "?").slice(0, 1).toUpperCase()
                            color: channelItem.modelData.isLive ? "#FF3B30" : "#0A84FF"
                            font.pixelSize: 18
                            font.weight: Font.Bold
                        }
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 4

                        Text {
                            Layout.fillWidth: true
                            text: channelItem.modelData.title || channelItem.modelData.name || "未知频道"
                            color: "#FFFFFF"
                            elide: Text.ElideRight
                            font.pixelSize: 14
                            font.weight: Font.Medium
                        }

                        RowLayout {
                            spacing: 8
                            Layout.fillWidth: true

                            Text {
                                text: channelItem.modelData.group || "未分组"
                                color: "#8E8E93"
                                font.pixelSize: 11
                                Layout.fillWidth: true
                            }

                            Rectangle {
                                visible: !!channelItem.modelData.isLive
                                Layout.preferredWidth: 40
                                Layout.preferredHeight: 20
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
                        }
                    }

                    Button {
                        text: root.playlistService.isFavorite(channelItem.modelData.url) ? "★" : "☆"
                        font.pixelSize: 18
                        background: Rectangle {
                            color: "transparent"
                            radius: 16
                        }
                        contentItem: Text {
                            text: parent.text
                            color: root.playlistService.isFavorite(channelItem.modelData.url) ? "#FFD700" : "#8E8E93"
                            font.pixelSize: 18
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        onClicked: root.playlistService.toggleFavorite(channelItem.modelData.url)

                        // 增大点击区域
                        TapHandler {
                            onTapped: root.playlistService.toggleFavorite(channelItem.modelData.url)
                        }
                    }
                }

                HoverHandler { id: channelMouse }
                TapHandler {
                    onTapped: root.playUrl(channelItem.modelData.url)
                }

                // 波纹效果
                Rectangle {
                    anchors.fill: parent
                    radius: 10
                    color: "#FFFFFF10"
                    opacity: 0
                    scale: 0.5
                    Behavior on opacity { NumberAnimation { duration: 200 } }
                    Behavior on scale { NumberAnimation { duration: 200 } }

                    TapHandler {
                        onTapped: {
                            parent.opacity = 0.3
                            parent.scale = 1.0
                            Qt.callLater(() => {
                                parent.opacity = 0
                                parent.scale = 0.5
                            }, 150)
                            root.playUrl(channelItem.modelData.url)
                        }
                    }
                }
            }
        }
    }
}