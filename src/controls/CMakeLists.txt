set(QML_SIGTON_SOURCES
    modules/Style/Style.qml
)

set(QML_SOURCES
    modules/Controls/CheckedAnimateButton.qml
    modules/Controls/CustomButton.qml
    modules/Controls/LineEdit.qml
    modules/Controls/SearchEdit.qml
    modules/Style/CustomButtonStyle.qml
    modules/Style/SearchEditStyle.qml
)

set(CONTROLS_SOURCES
    impl/orangeqmlinstance.cpp impl/orangeqmlinstance.h
)

set(CONTROLS_MPV_SOURCES
    impl/olmpvabstractitem.cpp impl/olmpvabstractitem.h
    impl/olmpvabstractitem_p.h impl/olmpvcontroller.cpp
    impl/olmpvcontroller.h impl/olmpvcontroller_p.h
    impl/olmpvitem.cpp impl/olmpvitem.h
    impl/olmpvrenderer.cpp impl/olmpvrenderer.h
    impl/olmpvproperties.h
)

set_source_files_properties(${QML_SIGTON_SOURCES}
    PROPERTIES QT_QML_SINGLETON_TYPE TRUE
)

qt_policy(SET QTP0001 NEW)
qt_add_qml_module(OrangeLiveControls
    URI "OL.Control"
    VERSION ${PROJECT_VERSION}
    QML_FILES ${QML_SOURCES} ${QML_SIGTON_SOURCES}
    SOURCES ${CONTROLS_SOURCES} ${CONTROLS_MPV_SOURCES}
    PLUGIN_TARGET OrangeLiveControls
    OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/OL/Control
)

target_include_directories(OrangeLiveControls
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/impl
    ${CMAKE_CURRENT_SOURCE_DIR}/../../third_party/libmpv/include
)

include(${CMAKE_CURRENT_LIST_DIR}/controls.cmake)

target_link_libraries(OrangeLiveControls PRIVATE
    Qt::Core
    Qt::Gui
    Qt::Qml
    Qt::Quick
    ${_VIDEO_DEPENDS_SDK}
)
