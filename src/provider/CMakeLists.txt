set(IMAGE_PROVIDER_SOURCES
    iconimageprovider.cpp iconimageprovider.h
    orangecontrolsplugin.cpp
)

qt_add_qml_module(OrangeLiveProvider
    URI "OrangeLiveProvider"
    PLUGIN_TARGET OrangeLiveProvider
    NO_GENERATE_PLUGIN_SOURCE
    CLASS_NAME OrangeLiveProviderExtensionPlugin
    SOURCES ${IMAGE_PROVIDER_SOURCES}
    OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/OrangeLiveProvider
)

target_link_libraries(OrangeLiveProvider PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
)
