import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

Rectangle {
    id: root
    color: "transparent"
    property var playlistManager: null

    // 添加源对话框
    Dialog {
        id: addSourceDialog
        title: "添加播放列表源"
        modal: true
        standardButtons: Dialog.Ok | Dialog.Cancel
        anchors.centerIn: parent

        ColumnLayout {
            spacing: 12
            width: 300

            TextField {
                id: sourceName
                placeholderText: "名称（例如：我的列表）"
                Layout.fillWidth: true
            }
            TextField {
                id: sourceUrl
                placeholderText: "m3u URL 或本地路径"
                Layout.fillWidth: true
            }
            CheckBox {
                id: setDefault
                text: "设为默认源"
                checked: false
            }
        }

        onAccepted: {
            if (sourceName.text.trim() !== "" && sourceUrl.text.trim() !== "") {
                playlistManager.addSource(sourceName.text, sourceUrl.text, setDefault.checked)
            }
            sourceName.text = ""
            sourceUrl.text = ""
            setDefault.checked = false
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 12

        Button {
            text: "添加新源"
            Layout.alignment: Qt.AlignRight
            background: Rectangle {
                color: "#0A84FF"
                radius: 8
            }
            contentItem: Text {
                text: "添加新源"
                color: "#FFFFFF"
                font.pixelSize: 13
            }
            onClicked: addSourceDialog.open()
        }

        ListView {
            id: sourcesList
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 8
            clip: true
            model: playlistManager ? playlistManager.sources : []

            delegate: Rectangle {
                required property var modelData
                required property int index
                width: sourcesList.width
                height: 80
                radius: 8
                color: "#2C2C2E"

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 6

                    RowLayout {
                        Layout.fillWidth: true
                        Text {
                            text: modelData.name
                            color: "#FFFFFF"
                            font.pixelSize: 14
                            font.weight: Font.Medium
                            Layout.fillWidth: true
                        }
                        Text {
                            visible: modelData.isDefault
                            text: "默认"
                            color: "#0A84FF"
                            font.pixelSize: 11
                        }
                    }

                    Text {
                        text: modelData.url || "（本地空源）"
                        color: "#8E8E93"
                        font.pixelSize: 11
                        elide: Text.ElideRight
                        Layout.fillWidth: true
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 8
                        Button {
                            text: "设为默认"
                            visible: !modelData.isDefault
                            background: Rectangle {
                                color: "#2C2C2E"
                                radius: 6
                                border.color: "#3A3A3C"
                                border.width: 0.5
                            }
                            contentItem: Text {
                                text: "设为默认"
                                color: "#0A84FF"
                                font.pixelSize: 11
                            }
                            onClicked: {
                                for (var i = 0; i < playlistManager.sources.length; ++i) {
                                    playlistManager.sources[i].isDefault = (i === index)
                                }
                                playlistManager.currentSourceIndex = index
                                playlistManager.saveSettings()
                            }
                        }
                        Button {
                            text: "刷新"
                            background: Rectangle {
                                color: "#2C2C2E"
                                radius: 6
                                border.color: "#3A3A3C"
                                border.width: 0.5
                            }
                            contentItem: Text {
                                text: "刷新"
                                color: "#FFFFFF"
                                font.pixelSize: 11
                            }
                            onClicked: {
                                playlistManager.loadSourceChannels(index)
                            }
                        }
                        Button {
                            text: "删除"
                            visible: !modelData.isDefault
                            background: Rectangle {
                                color: "#FF3B3020"
                                radius: 6
                                border.color: "#FF3B30"
                                border.width: 0.5
                            }
                            contentItem: Text {
                                text: "删除"
                                color: "#FF3B30"
                                font.pixelSize: 11
                            }
                            onClicked: {
                                playlistManager.removeSource(index)
                            }
                        }
                    }
                }
            }

            Label {
                anchors.centerIn: parent
                visible: sourcesList.count === 0
                text: "暂无播放列表源，点击上方按钮添加"
                color: "#8E8E93"
            }
        }
    }
}