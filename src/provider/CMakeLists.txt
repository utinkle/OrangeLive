set(IMAGE_PROVIDER_SOURCES
    iconimageprovider.cpp iconimageprovider.h
    orangecontrolsplugin.cpp
)

set(IMAGE_PROVIDER_QML_SOURCES
    qml/OLIcon.qml
)

qt_policy(SET QTP0001 NEW)
find_package(Qt6 REQUIRED COMPONENTS Core)

qt_add_qml_module(OrangeLiveProvider
    URI "OL.Provider"
    VERSION ${PROJECT_VERSION}
    PLUGIN_TARGET OrangeLiveProvider
    NO_GENERATE_PLUGIN_SOURCE
    NO_PLUGIN_OPTIONAL
    CLASS_NAME OrangeLiveProviderExtensionPlugin
    SOURCES ${IMAGE_PROVIDER_SOURCES}
    QML_FILES ${IMAGE_PROVIDER_QML_SOURCES}
    OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/OL/Provider
    SOURCES olfonticonengine.h olfonticonengine.cpp
    SOURCES olfonticonmanager.h olfonticonmanager.cpp
)

target_link_libraries(OrangeLiveProvider PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
)
target_link_libraries(OrangeLiveProvider PRIVATE Qt6::Core)
