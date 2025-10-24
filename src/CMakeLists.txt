# 必须要让主程序包含其他子项目，他才能将qt的依赖项加到android包中
include(main/main.cmake)

add_subdirectory(controls)
add_subdirectory(core)
add_subdirectory(provider)
