cmake_minimum_required(VERSION 3.16)
project(OLHomePage LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)

file(GLOB preload_qml_resource_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} qml/preload/*.qml)

file(GLOB preload_img_resource_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} resources/*)

qt_add_plugin(OLHomePagePreload
    SHARED
    PLUGIN_TYPE OLPlugin
    CLASS_NAME OLHomePagePreload
    olhomepagepreload.h olhomepagepreload.cpp
)

set(PROJECT_APP_ID "OrangeLive")
set(PROJECT_APP_DESCRIPTOR "OLHomePage")
set(PROJECT_APP_VERSION "1.0.0")

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/preload.json.in
    ${CMAKE_CURRENT_BINARY_DIR}/preload.json
    @ONLY
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/entity.json.in
    ${CMAKE_CURRENT_BINARY_DIR}/entity.json
    @ONLY
)

target_include_directories(OLHomePagePreload PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

set_target_properties(OLHomePagePreload PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY
    ${CMAKE_BINARY_DIR}/Extension/Modules
)

qt_add_resources(OLHomePagePreload "preload_qml"
    PREFIX
        "/OL.HomePage"
    FILES
        ${preload_qml_resource_files}
)

qt_add_resources(OLHomePagePreload "preload_img"
    PREFIX
        "/OL.HomePage"
    FILES
        ${preload_img_resource_files}
)

target_link_libraries(OLHomePagePreload PRIVATE
    Qt::Quick
    Qt::QuickControls2
    CCQuickStartup
)

qt_add_plugin(OLHomePageEntity
    SHARED
    PLUGIN_TYPE OLPlugin
    CLASS_NAME OLHomePageEntity
    olhomepageentity.h olhomepageentity.cpp
)

target_include_directories(OLHomePageEntity PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

set_target_properties(OLHomePageEntity PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY
    ${CMAKE_BINARY_DIR}/Extension/Modules
)

file(GLOB entity_qml_resource_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} qml/entity/*.qml)

qt_add_resources(OLHomePageEntity "entity_qml"
    PREFIX
        "/OL.HomePage"
    FILES
        ${entity_qml_resource_files}
)

target_compile_definitions(OLHomePageEntity PRIVATE ORANGE_ARCH_STRING="${ANDROID_ABI}")

target_link_libraries(OLHomePageEntity PRIVATE
    Qt::Quick
    Qt::QuickControls2
    CCQuickStartup
)
