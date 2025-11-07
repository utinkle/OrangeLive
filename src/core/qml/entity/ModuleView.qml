import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts

import OL.Core
import OL.Control

Item {
    id: root

    enum ModuleLayoutState {
        InFullscreen,
        InLayout
    }

    readonly property alias viewLayout: contentView.layoutState
    property real statusBarHeight: viewLayout === ModuleView.ModuleLayoutState.InLayout ? Style.statusBarHeight
                                                                                        : 0
    property real navigationBarHeight: viewLayout === ModuleView.ModuleLayoutState.InLayout ? Style.navigationBarHeight
                                                                                            : 0
    property real contentMargin: viewLayout === ModuleView.ModuleLayoutState.InLayout ? 20 : 0
    property real contentSpacing: viewLayout === ModuleView.ModuleLayoutState.InLayout ? 20 : 0

    Behavior on contentMargin {
        NumberAnimation { duration: 200; easing.type: Easing.InCubic }
    }

    Behavior on contentSpacing {
        NumberAnimation { duration: 200; easing.type: Easing.InCubic }
    }

    Behavior on statusBarHeight {
        NumberAnimation { duration: 200; easing.type: Easing.InCubic }
    }

    Behavior on navigationBarHeight {
        NumberAnimation { duration: 200; easing.type: Easing.InCubic }
    }

    RowLayout {
        id: centralLayout

        anchors.fill: parent
        spacing: root.contentSpacing

        Rectangle {
            id: moduleLeftNavigation

            Layout.preferredWidth: viewLayout === ModuleView.ModuleLayoutState.InLayout ? 60 : 0
            Layout.fillHeight: true
            Layout.leftMargin: contentMargin
            Layout.topMargin: statusBarHeight + contentMargin
            Layout.bottomMargin: navigationBarHeight + contentMargin

            radius: 8
            color: Qt.rgba(0.25, 0.25, 0.25, 1.0)
            visible: Layout.preferredWidth !== 0

            Behavior on Layout.preferredWidth {
                NumberAnimation { duration: 200; easing.type: Easing.InCubic }
            }
        }

        ColumnLayout {
            id: moduleContent

            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.rightMargin: contentMargin
            Layout.topMargin: statusBarHeight + contentMargin
            Layout.bottomMargin: navigationBarHeight + contentMargin

            spacing: root.contentSpacing

            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                spacing: contentSpacing

                Item {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    FlipSwipeView {
                        id: contentView

                        clip: true
                        readonly property real windowAspectRatio: Window.window.width / Window.window.height
                        property int layoutState: ModuleView.ModuleLayoutState.InFullscreen

                        width: Math.min(parent.width, parent.height * windowAspectRatio)
                        height: width * 1.0 / windowAspectRatio
                        anchors.centerIn: parent

                        contentData: OLModuleLoader.runingModuleItems
                        orientation: Qt.Horizontal
                        interactive: layoutState === ModuleView.ModuleLayoutState.InLayout

                        Component.onCompleted: {
                            OLModuleLoader.moduleView = contentView
                        }
                    }
                }

                Item {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 6
                    visible: viewLayout === ModuleView.ModuleLayoutState.InLayout

                    PageIndicator {
                        id: indicator

                        anchors.centerIn: parent
                        currentIndex: contentView.currentIndex
                        count: contentView.count

                        delegate: Loader {
                            required property int index
                            active: Math.abs(index - indicator.currentIndex) < 2

                            sourceComponent: Rectangle {
                                implicitWidth: 60
                                implicitHeight: 6

                                radius: implicitHeight / 2
                                color: Qt.rgba(0.65, 0.65, 0.65)

                                opacity: index === indicator.currentIndex ? 0.95 : indicator.pressed ? 0.7 : 0.45
                                Behavior on opacity { OpacityAnimator { duration: 100 } }
                            }
                        }
                    }
                }
            }

            Rectangle {
                id: moduleBottomNavigation

                Layout.fillWidth: true
                Layout.preferredHeight: viewLayout === ModuleView.ModuleLayoutState.InLayout ? 60 : 0

                radius: 8
                color: Qt.rgba(0.25, 0.25, 0.25, 1.0)
                visible: Layout.preferredHeight !== 0

                Behavior on Layout.preferredHeight {
                    NumberAnimation { duration: 200; easing.type: Easing.InCubic }
                }
            }
        }
    }
}
