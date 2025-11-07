cmake_minimum_required(VERSION 3.16)
project(OrangeLive VERSION 1.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

option(OL_BUILD_EXAMPLE_MODULES "Build the example modules" OFF)

find_package(Qt6 REQUIRED COMPONENTS Gui Qml Quick QuickControls2)

qt_standard_project_setup()

set(OL_MODULE_OUTPUT_DIR ${CMAKE_BINARY_DIR}/Extension/Modules)

add_subdirectory(src)
add_subdirectory(third_party/CCQuickStartup)
add_subdirectory(third_party/MobileUI/)
add_subdirectory(modules/components/)
add_subdirectory(modules/activity/)

