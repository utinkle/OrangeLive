set(QML_SOURCES
    modules/Controls/CheckedAnimateButton.qml
    modules/Controls/CustomButton.qml
    modules/Controls/LineEdit.qml
    modules/Controls/SearchEdit.qml
    modules/Controls/TokIcon.qml
    modules/Style/Style.qml
    modules/Style/CustomButtonStyle.qml
    modules/Style/SearchEditStyle.qml
)

set(CONTROLS_SOURCES
    impl/orangeqmlinstance.cpp impl/orangeqmlinstance.h
)

qt_policy(SET QTP0001 NEW)
qt_add_qml_module(OrangeLiveControls
    URI "OrangeLive"
    VERSION ${PROJECT_VERSION}
    QML_FILES ${QML_SOURCES}
    SOURCES ${CONTROLS_SOURCES}
    PLUGIN_TARGET OrangeLiveControls
    OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/OrangeLive
)

add_dependencies(OrangeLiveControls OrangeLiveProvider)

set_source_files_properties(modules/Style/Style.qml
    PROPERTIES QT_QML_SINGLETON_TYPE TRUE
)

target_include_directories(OrangeLiveControls
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/impl
)

target_link_libraries(OrangeLiveControls PRIVATE
    Qt::Core
    Qt::Gui
    Qt::Qml
    Qt::Quick
)
