cmake_minimum_required(VERSION 3.16)
project(OrangeLive VERSION 1.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Gui Qml Quick QuickControls2)

qt_standard_project_setup()

add_subdirectory(src)
add_subdirectory(third_party/CCQuickStartup)
add_subdirectory(third_party/MobileUI/)

