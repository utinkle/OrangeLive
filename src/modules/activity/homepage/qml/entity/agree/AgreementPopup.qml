import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material

import CCStartup

Popup {
    id: root

    signal accepted()

    width: parent.width * 0.6
    height: Math.min(parent.height * 0.8, 500)

    modal: true
    closePolicy: Popup.NoAutoClose
    margins: 20

    background: Rectangle {
        color: "white"
        radius: 16
    }

    enter: Transition {
        NumberAnimation {
            property: "scale"
            from: 0.0
            to: 1.0
            easing.type: Easing.OutCubic
        }
    }

    exit: Transition {
        NumberAnimation {
            property: "scale"
            from: 1.0
            to: 0.0
            easing.type: Easing.InBack
        }
    }

    ColumnLayout {
        spacing: 20
        anchors.fill: parent
        clip: true

        AgTitlePane {
            Layout.fillWidth: true
            Layout.preferredHeight: 40
        }

        Component {
            id: detailComp
            AgDetailPage { }
        }

        StackView {
            id: agreeStackView
            Layout.fillWidth: true
            Layout.fillHeight: true

            background: Rectangle {
                color: Qt.rgba(0.0, 0.0, 0.0, 0.05)
                radius: 8
            }

            initialItem: AgInitPage {
                id: agreeStackInitPage
                onUserAgreementLinkActivated: {
                    agreeStackView.push(detailComp, { textSource: "qrc:/OLHomePage/resources/files/user_agreement.txt" })
                }

                onUserRightsResponsibilitiesLinkActivated: {
                    agreeStackView.push(detailComp, { textSource: "qrc:/OLHomePage/resources/files/user_rights_responsibilities.txt" })
                }

                onPrivacyPolicyLinkActivated: {
                    agreeStackView.push(detailComp, { textSource: "qrc:/OLHomePage/resources/files/privacy_policy.txt" })
                }

                onPluginSafetyLinkActivated: {
                    agreeStackView.push(detailComp, { textSource: "qrc:/OLHomePage/resources/files/plugin_safety.txt" })
                }

                onHowWeOperateLinkActivated: {
                    agreeStackView.push(detailComp, { textSource: "qrc:/OLHomePage/resources/files/how_we_operate.txt" })
                }
            }
        }

        SwipeView {
            Layout.fillWidth: true
            Layout.preferredHeight: 50

            currentIndex: agreeStackInitPage === agreeStackView.currentItem ? 0 : 1
            interactive: false
            orientation: Qt.Horizontal
            background: Rectangle {
                radius: 8
                color: Qt.rgba(0.0, 0.0, 0.0, 0.05)
            }

            AgInitButtonPane {
                agreeEnabled: agreeStackInitPage.agreeAllChecked
                onAgreeButtonClicked: {
                    root.accepted()
                    root.close()
                }

                onQuitButtonClicked: {
                    Qt.quit()
                }
            }

            AgDetailButtonPane {
                onAgreeButtonClicked: {
                    agreeStackView.pop()
                }

                onBackButtonClicked: {
                    agreeStackView.pop()
                }
            }
        }
    }
}
