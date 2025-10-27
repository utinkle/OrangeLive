cmake_minimum_required(VERSION 3.16)
project(OrangeLivePlugins LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)

file(GLOB preload_qml_resource_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} qml/preload/*.qml)

file(GLOB preload_img_resource_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} resources/*)

qt_add_plugin(OrangeLiveCorePreload
    SHARED
    CLASS_NAME OrangeLiveCorePreload
    orangelivecorepreload.h orangelivecorepreload.cpp
)

set(PROJECT_APP_ID "OrangeLive")
set(PROJECT_APP_DESCRIPTOR "OrangeLiveCore")
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

target_include_directories(OrangeLiveCorePreload PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

set_target_properties(OrangeLiveCorePreload PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY
    ${CMAKE_BINARY_DIR}/Extension/Core/Preload
)

qt_add_resources(OrangeLiveCorePreload "preload_qml"
    PREFIX
        "/OL.Core"
    FILES
        ${preload_qml_resource_files}
)

qt_add_resources(OrangeLiveCorePreload "preload_img"
    PREFIX
        "/OL.Core"
    FILES
        ${preload_img_resource_files}
)

target_compile_definitions(OrangeLiveCorePreload PRIVATE ORANGE_ARCH_STRING="${ANDROID_ABI}")

target_link_libraries(OrangeLiveCorePreload PRIVATE
    Qt::Quick
    Qt::QuickControls2
    CCQuickStartup
)

find_package(Qt6 REQUIRED COMPONENTS Quick)

qt_add_plugin(OrangeLiveCoreEntity
    SHARED
    CLASS_NAME OrangeLiveCoreEntity
    orangelivecoreentity.cpp
    orangelivecoreentity.h
    olmoduleinformation.h
    olmoduleversioncontrol.h
    olmoduleversioncontrol.cpp
    olmoduleloader.h
    olmoduleloader.cpp
    olmoduletemplate.h
    olmoduletemplate.cpp
)

target_include_directories(OrangeLiveCoreEntity PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

set_target_properties(OrangeLiveCoreEntity PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY
    ${CMAKE_BINARY_DIR}/Extension/Entity
)

file(GLOB entity_qml_resource_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} qml/entity/*.qml)

qt_add_resources(OrangeLiveCoreEntity "entity_qml"
    PREFIX
        "/OL.Core"
    FILES
        ${entity_qml_resource_files}
)

target_compile_definitions(OrangeLiveCoreEntity PRIVATE ORANGE_ARCH_STRING="${ANDROID_ABI}")

target_link_libraries(OrangeLiveCoreEntity PRIVATE
    Qt::Quick
    Qt::QuickControls2
    CCQuickStartup
)
target_link_libraries(OrangeLiveCoreEntity PRIVATE Qt6::Quick)

