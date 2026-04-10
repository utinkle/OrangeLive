import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import OL.Control
import OL.HomePage

Item {
    id: root
    required property int index
    required property string videoFile
    property bool controlsVisible: true
    property double holdSpeed: 1.0
    property double pinchBaseZoom: 0.0
    property string currentChannelUrl: ""
    property bool showDiagnostics: false
    property alias mediaTitle: mpv.mediaTitle
    property string displayTitle: ""

    Layout.fillWidth: true
    Layout.fillHeight: true

    MpvItem {
        id: mpv
        z: -5

        anchors.fill: parent
        onReady: {
            if (root.videoFile.length > 0) {
                root.loadFromUrl(root.videoFile)
            }
        }

        onEndFile: reason => {
            if (reason === "eof") {
                root.nextRequested(root.currentChannelUrl)
            }
        }

        function diagnosticText() {
            return `state=${playbackState}\n` +
                `liveMode=${liveMode}\n` +
                `streamType=${streamType}\n` +
                `seekable=${seekable}\n` +
                `cacheDuration=${cacheDuration.toFixed(2)}\n` +
                `demuxerCache=${demuxerCacheTime.toFixed(2)}\n` +
                `networkKBps=${networkSpeedKbps.toFixed(2)}\n` +
                `droppedFrames=${droppedFrames}\n` +
                `avSync=${avSync.toFixed(3)}\n` +
                `playbackHealth=${playbackHealth}`
        }

        TapHandler {
            acceptedButtons: Qt.LeftButton
            onTapped: {
                root.controlsVisible = !root.controlsVisible
            }
            onDoubleTapped: eventPoint => {
                if (mpv.liveMode) {
                    return
                }
                const delta = eventPoint.position.x < mpv.width / 2 ? -10 : 10
                mpv.position = Math.max(0, Math.min(mpv.duration, mpv.position + delta))
            }
        }

        TapHandler {
            acceptedButtons: Qt.LeftButton
            longPressThreshold: 300
            onLongPressed: {
                if (mpv.liveMode) {
                    return
                }
                root.holdSpeed = mpv.playbackRate
                mpv.playbackRate = 2.0
            }
            onPressedChanged: {
                if (!pressed && mpv.playbackRate !== root.holdSpeed) {
                    mpv.playbackRate = root.holdSpeed
                }
            }
        }

        PinchHandler {
            minimumScale: 0.5
            maximumScale: 3.0
            onActiveChanged: {
                if (active) {
                    root.pinchBaseZoom = mpv.videoZoom
                }
            }
            onScaleChanged: {
                const targetZoom = root.pinchBaseZoom + (scale - 1.0)
                mpv.videoZoom = Math.max(-1.0, Math.min(3.0, targetZoom))

                if (mpv.videoZoom < 0.2) {
                    mpv.videoFillMode = "fit"
                } else if (mpv.videoZoom < 0.8) {
                    mpv.videoFillMode = "fill"
                } else {
                    mpv.videoFillMode = "crop"
                }
            }
        }
    }

    signal nextRequested(string currentUrl)

    function loadFromUrl(url) {
        currentChannelUrl = url
        mpv.loadFile(url)
        mpv.pause = false
    }

    function pausePlayback() {
        if (mpv.playbackState !== "error" && mpv.playbackState !== "opening") {
            mpv.pause = true
        }
    }

    function resumePlayback() {
        if (mpv.playbackState !== "error" && mpv.playbackState !== "opening") {
            mpv.pause = false
        }
    }

    function togglePlayback() {
        if (mpv.playbackState === "error" || mpv.playbackState === "opening")
            return

        mpv.pause = !mpv.pause
    }

    ClipboardHelper {
        id: clipboardHelper
    }

    AndroidPlaybackHelper {
        id: androidPlaybackHelper
    }

    Rectangle {
        anchors.centerIn: parent
        width: 72
        height: 72
        radius: 36
        color: "#CC000000"
        visible: mpv.pause && mpv.playbackState === "playing"
        opacity: 0.9
        z: 10

        Image {
            anchors.centerIn: parent
            width: 32
            height: 32
            source: "qrc:/OLHomePage/resources/images/play_icon.png"
        }

        TapHandler {
            onTapped: {
                root.resumePlayback()
            }
        }
    }

    // 加载状态指示器
    Rectangle {
        anchors.centerIn: parent
        visible: mpv.playbackState === "opening" || mpv.playbackState === "buffering"
        width: 140
        height: 60
        radius: 24
        color: "#CC1C1C1E"

        RowLayout {
            anchors.fill: parent
            spacing: 12

            BusyIndicator {
                Layout.leftMargin: 20
                Layout.alignment: Qt.AlignVCenter
                Layout.preferredWidth: 24
                Layout.preferredHeight: 24
                running: true

                contentItem: Rectangle {
                    width: 24
                    height: 24
                    color: "transparent"

                    Rectangle {
                        width: 20
                        height: 20
                        radius: 10
                        color: "transparent"
                        border.width: 2
                        border.color: "#0A84FF"
                        opacity: 0.8
                    }
                }
            }

            Text {
                Layout.alignment: Qt.AlignVCenter

                text: mpv.playbackState === "buffering" ? "缓冲中..." : "加载中..."
                color: "#FFFFFF"
                font.pixelSize: 14
            }
        }
    }

    // 错误提示
    Rectangle {
        anchors.centerIn: parent
        visible: mpv.playbackState === "error"
        width: 320
        height: 160
        radius: 16
        color: "#E6353535"
        border.width: 1
        border.color: "#FF3B30"

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 20
            spacing: 12

            RowLayout {
                Layout.fillWidth: true

                Rectangle {
                    width: 32
                    height: 32
                    radius: 16
                    color: "#FF3B30"

                    Text {
                        anchors.centerIn: parent
                        text: "!"
                        color: "#FFFFFF"
                        font.bold: true
                        font.pixelSize: 20
                    }
                }

                Text {
                    text: "播放失败"
                    color: "#FF3B30"
                    font.bold: true
                    font.pixelSize: 16
                    Layout.fillWidth: true
                }
            }

            Text {
                text: mpv.errorMessage.length > 0 ? mpv.errorMessage : "未知错误，请检查网络或视频源"
                color: "#FFFFFF"
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
                font.pixelSize: 13
            }

            Button {
                text: "重试"
                Layout.alignment: Qt.AlignRight
                background: Rectangle {
                    color: "#0A84FF"
                    radius: 8
                }
                contentItem: Text {
                    text: "重试"
                    color: "#FFFFFF"
                    font.pixelSize: 14
                    font.weight: Font.Medium
                }
                onClicked: mpv.retry()
            }
        }
    }

    Rectangle {
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 20
        width: 300
        height: 320
        radius: 16
        visible: root.showDiagnostics
        color: "#E62C2C2E"
        border.width: 0.5
        border.color: "#3A3A3C"

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 16
            spacing: 12

            RowLayout {
                Layout.fillWidth: true

                Text {
                    text: "诊断面板"
                    color: "#FFFFFF"
                    font.bold: true
                    font.pixelSize: 16
                    Layout.fillWidth: true
                }

                Button {
                    width: 28
                    height: 28
                    background: Rectangle {
                        color: "transparent"
                        radius: 6
                    }
                    contentItem: Text {
                        text: "❎"
                        color: "#8E8E93"
                        font.pixelSize: 16
                    }
                    onClicked: root.showDiagnostics = false
                }
            }

            ScrollView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true

                TextArea {
                    readOnly: true
                    text: mpv.diagnosticText()
                    color: "#8E8E93"
                    wrapMode: Text.WrapAnywhere
                    selectByMouse: true
                    font.pixelSize: 11
                    font.family: "Courier New, monospace"
                    background: Rectangle {
                        color: "#1C1C1E"
                        radius: 8
                    }
                    padding: 12
                }
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 12

                Button {
                    text: "复制"
                    Layout.fillWidth: true
                    background: Rectangle {
                        color: "#0A84FF"
                        radius: 8
                    }
                    contentItem: Text {
                        text: "复制"
                        color: "#FFFFFF"
                        font.pixelSize: 13
                        font.weight: Font.Medium
                    }
                    onClicked: clipboardHelper.copyText(mpv.diagnosticText())
                }

                Button {
                    text: "关闭"
                    Layout.fillWidth: true
                    background: Rectangle {
                        color: "#2C2C2E"
                        radius: 8
                    }
                    contentItem: Text {
                        text: "关闭"
                        color: "#FFFFFF"
                        font.pixelSize: 13
                    }
                    onClicked: root.showDiagnostics = false
                }
            }
        }
    }

    // 底部控制栏
    Rectangle {
        width: parent.width
        height: 90 + Style.navigationBarHeight
        visible: root.controlsVisible
        color: "#D0000000"
        anchors.bottom: parent.bottom

        // 渐变背景增加层次感
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#00000000" }
            GradientStop { position: 0.3; color: "#80000000" }
            GradientStop { position: 1.0; color: "#D0000000" }
        }

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 12
            anchors.bottomMargin: Style.navigationBarHeight
            spacing: 8

            // 进度条区域
            RowLayout {
                Layout.leftMargin: 16
                Layout.rightMargin: 16
                Layout.fillWidth: true
                spacing: 12

                // 播放/暂停按钮
                Rectangle {
                    width: 32
                    height: 32
                    radius: 16
                    color: "#2C2C2E"
                    visible: mpv.playbackState === "playing" || mpv.playbackState === "paused"

                    Image {
                        anchors.centerIn: parent
                        width: 20
                        height: 20
                        source: mpv.pause ? "qrc:/OLHomePage/resources/images/play_icon.png" : "qrc:/OLHomePage/resources/images/pause_icon.png"
                    }

                    TapHandler {
                        onTapped: {
                            root.togglePlayback()
                        }
                    }
                }

                Text {
                    text: mpv.liveMode ? "LIVE" : `${mpv.formattedPosition} / ${mpv.formattedDuration}`
                    color: "#FFFFFF"
                    font.bold: mpv.liveMode
                    font.pixelSize: 12
                    font.weight: mpv.liveMode ? Font.Bold : Font.Normal
                }

                Slider {
                    id: slider
                    visible: !mpv.liveMode
                    enabled: !mpv.liveMode
                    from: 0
                    to: mpv.duration
                    value: mpv.position
                    onMoved: mpv.position = value

                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    // 自定义Slider样式
                    background: Rectangle {
                        height: 3
                        radius: 1.5
                        color: "#3A3A3C"
                        Rectangle {
                            width: slider.visualPosition * parent.width
                            height: parent.height
                            radius: 1.5
                            color: "#0A84FF"
                        }
                    }
                    handle: Rectangle {
                        width: 16
                        height: 16
                        radius: 8
                        color: "#0A84FF"
                        visible: slider.pressed || slider.hovered
                        border.width: 2
                        border.color: "#FFFFFF"
                    }
                }
            }

            // 控制按钮组
            RowLayout {
                Layout.leftMargin: 16
                Layout.rightMargin: 16
                Layout.fillWidth: true
                spacing: 12

                ComboBox {
                    Layout.preferredHeight: 30
                    visible: !mpv.liveMode
                    model: [0.5, 1.0, 1.25, 1.5, 2.0]
                    currentIndex: 1
                    onActivated: mpv.playbackRate = Number(currentText)

                    background: Rectangle {
                        color: "#2C2C2E"
                        radius: 6
                        border.color: "#3A3A3C"
                        border.width: 0.5
                    }
                    contentItem: Text {
                        text: parent.currentText + "x"
                        color: "#FFFFFF"
                        font.pixelSize: 12
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                    }
                    indicator: null
                }

                ComboBox {
                    Layout.preferredWidth: 60
                    Layout.preferredHeight: 30
                    model: ["软解", "硬解"]
                    currentIndex: mpv.hwdecMode === "no" ? 0 : 1
                    onActivated: mpv.hwdecMode = currentIndex === 0 ? "no" : "auto"

                    background: Rectangle {
                        color: "#2C2C2E"
                        radius: 6
                        border.color: "#3A3A3C"
                        border.width: 0.5
                    }
                    contentItem: Text {
                        text: parent.currentText
                        color: "#FFFFFF"
                        font.pixelSize: 12
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                    }
                    indicator: null

                    ToolTip.visible: hovered
                    ToolTip.text: `当前解码:${mpv.hwdecCurrent}`
                    ToolTip.delay: 500
                }

                Button {
                    text: mpv.videoFillMode === "fit" ? "适应" : (mpv.videoFillMode === "fill" ? "填充" : "裁剪")
                    background: Rectangle {
                        color: parent.pressed ? "#3A3A3C" : "#2C2C2E"
                        radius: 6
                        border.color: "#3A3A3C"
                        border.width: 0.5
                    }
                    contentItem: Text {
                        text: parent.text
                        color: "#FFFFFF"
                        font.pixelSize: 12
                    }
                    onClicked: {
                        if (mpv.videoFillMode === "fit") {
                            mpv.videoFillMode = "fill"
                        } else if (mpv.videoFillMode === "fill") {
                            mpv.videoFillMode = "crop"
                        } else {
                            mpv.videoFillMode = "fit"
                        }
                    }
                }

                Button {
                    text: root.showDiagnostics ? "诊断开" : "诊断"
                    background: Rectangle {
                        color: parent.pressed ? "#3A3A3C" : (root.showDiagnostics ? "#0A84FF" : "#2C2C2E")
                        radius: 6
                        border.color: "#3A3A3C"
                        border.width: 0.5
                    }
                    contentItem: Text {
                        text: parent.text
                        color: root.showDiagnostics ? "#FFFFFF" : "#8E8E93"
                        font.pixelSize: 12
                    }
                    onClicked: root.showDiagnostics = !root.showDiagnostics
                }

                Button {
                    text: "PiP"
                    enabled: androidPlaybackHelper.pictureInPictureSupported
                    background: Rectangle {
                        color: parent.enabled ? (parent.pressed ? "#3A3A3C" : "#0A84FF") : "#2C2C2E"
                        radius: 6
                        border.color: "#3A3A3C"
                        border.width: 0.5
                    }
                    contentItem: Text {
                        text: parent.text
                        color: parent.enabled ? "#FFFFFF" : "#5A5A5E"
                        font.pixelSize: 12
                    }
                    onClicked: androidPlaybackHelper.requestPictureInPicture()
                }

                Item { Layout.fillWidth: true }

                // 网络状态指示
                Rectangle {
                    visible: mpv.networkSpeedKbps > 0
                    Layout.preferredHeight: 28
                    Layout.preferredWidth: statusRow.width + 16
                    radius: 6
                    color: "#2C2C2E"

                    RowLayout {
                        id: statusRow
                        anchors.centerIn: parent
                        spacing: 6

                        Rectangle {
                            Layout.alignment: Qt.AlignVCenter
                            Layout.preferredWidth: 6
                            Layout.preferredHeight: 6
                            radius: 3
                            color: mpv.playbackHealth === "good" ? "#34C759" : (mpv.playbackHealth === "fair" ? "#FF9500" : "#FF3B30")
                        }

                        Text {
                            id: contentItem
                            text: `${mpv.networkSpeedKbps.toFixed(0)} KB/s`
                            color: "#8E8E93"
                            font.pixelSize: 10
                        }
                    }
                }
            }
        }
    }
}