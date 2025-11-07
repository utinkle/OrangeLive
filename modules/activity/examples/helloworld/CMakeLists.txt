cmake_minimum_required(VERSION 3.16)
project(OLHelloWorld LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)

file(GLOB preload_qml_resource_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} qml/preload/*.qml)

qt_add_plugin(OLHelloWorldPreload
    SHARED
    PLUGIN_TYPE OLPlugin
    CLASS_NAME OLHelloWorldPreload
    olhelloworldpreload.h olhelloworldpreload.cpp
)

set(PROJECT_APP_ID "OrangeLive")
set(PROJECT_APP_DESCRIPTOR "OLHelloWorld")
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

target_include_directories(OLHelloWorldPreload PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

set_target_properties(OLHelloWorldPreload PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY
    ${CMAKE_BINARY_DIR}/Extension/Modules
)

qt_add_resources(OLHelloWorldPreload "preload_qml"
    PREFIX
        "/OL.HelloWorld"
    FILES
        ${preload_qml_resource_files}
)

target_link_libraries(OLHelloWorldPreload PRIVATE
    Qt::Quick
    Qt::QuickControls2
    CCQuickStartup
)

qt_add_plugin(OLHelloWorldEntity
    SHARED
    PLUGIN_TYPE OLPlugin
    CLASS_NAME OLHelloWorldEntity
    olhelloworldentity.h olhelloworldentity.cpp
)

target_include_directories(OLHelloWorldEntity PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

set_target_properties(OLHelloWorldEntity PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY
    ${OL_MODULE_OUTPUT_DIR}
)

file(GLOB entity_qml_resource_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} qml/entity/*.qml)
file(GLOB entity_img_resource_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} resources/*)

qt_add_resources(OLHelloWorldEntity "entity_qml"
    PREFIX
        "/OL.HelloWorld"
    FILES
        ${entity_qml_resource_files}
)

qt_add_resources(OLHelloWorldEntity "entity_img"
    PREFIX
        "/OLHelloWorld"
    FILES
        ${entity_img_resource_files}
)

target_link_libraries(OLHelloWorldEntity PRIVATE
    Qt::Quick
    Qt::QuickControls2
    CCQuickStartup
)
