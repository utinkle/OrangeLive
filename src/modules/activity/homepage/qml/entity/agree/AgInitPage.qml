import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ColumnLayout {
    id: root

    signal userAgreementLinkActivated()
    signal userRightsResponsibilitiesLinkActivated()
    signal privacyPolicyLinkActivated()
    signal pluginSafetyLinkActivated()
    signal howWeOperateLinkActivated()

    property alias agreeAllChecked: agreeAllCheckBox.checked

    ScrollView {
        id: scrollView
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.margins: 10
        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
        ScrollBar.horizontal.interactive: false

        ColumnLayout {
            width: scrollView.width
            spacing: 20

            Text {
                Layout.topMargin: 10
                Layout.alignment: Qt.AlignHCenter
                text: "欢迎使用「橙子工具台」！"

                font.bold: true
                font.pixelSize: 16
                color: Material.accentColor
            }

            Text {
                Layout.alignment: Qt.AlignHCenter
                text: "在开始前，请花一点时间阅读我们的用户协议\n我们珍视您的信任，并致力于保护您的隐私。"
                font.pixelSize: 14
                color: Qt.rgba(0, 0, 0, 0.6)
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: childrenRect.height + 20

                color: "transparent"
                border.color: Qt.rgba(0.2, 0.2, 0.2, 0.1)
                border.width: 1
                radius: 8

                ColumnLayout {
                    width: parent.width - 40
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: parent.top
                    anchors.topMargin: 10
                    anchors.leftMargin: 20
                    spacing: 2

                    Text {
                        text: "<a href=\"http://test.url\" target=\"_self\">• 用户协议</a>"
                        onLinkActivated: {
                            root.userAgreementLinkActivated()
                        }
                    }

                    Text {
                        text: "<a href=\"http://test.url\" target=\"_self\">• 隐私政策</a>"
                        onLinkActivated: {
                            root.privacyPolicyLinkActivated()
                        }
                    }

                    Text {
                        text: "<a href=\"http://test.url\" target=\"_self\">• 插件生态与安全</a>"
                        onLinkActivated: {
                            root.pluginSafetyLinkActivated()
                        }
                    }

                    Text {
                        text: "<a href=\"http://test.url\" target=\"_self\">• 程序如何使用</a>"
                        onLinkActivated: {
                            root.howWeOperateLinkActivated()
                        }
                    }

                    Text {
                        text: "<a href=\"http://test.url\" target=\"_self\">• 您的权利与责任</a>"
                        onLinkActivated: {
                            root.userRightsResponsibilitiesLinkActivated()
                        }
                    }

                }
            }
        }
    }

    Rectangle {
        Layout.preferredHeight: 1
        Layout.fillWidth: true

        color: Qt.rgba(0.2, 0.2, 0.2, 0.1)
    }

    CheckBox {
        id: agreeAllCheckBox
        Layout.leftMargin: 10
        text: "我已阅读并同意上述内容"
    }
}
