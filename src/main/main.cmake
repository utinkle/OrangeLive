set(CMAKE_AUTORCC ON)
qt_add_executable(OrangeLive WIN32 MACOSX_BUNDLE
    ${CMAKE_CURRENT_LIST_DIR}/application.qrc
    ${CMAKE_CURRENT_LIST_DIR}/main.cpp
)

# 这里的 OpenSSL 路径为设置里面的 OpenSSL 项目路径
if (OL_OPENSSL_ANDROID_DIR)
    message(${OL_OPENSSL_ANDROID_DIR} "********************")
    include(${OL_OPENSSL_ANDROID_DIR}/android_openssl.cmake)
    add_android_openssl_libraries(OrangeLive)
endif()

include(${CMAKE_SOURCE_DIR}/src/controls/controls.cmake)
add_control_android_dependencies(OrangeLive)

set(EXTENSION_TARGETS
    $<TARGET_FILE:OrangeLiveCorePreload>
    $<TARGET_FILE:OrangeLiveCoreEntity>
    $<TARGET_FILE:OrangeLiveControls>
    $<TARGET_FILE:OrangeLiveProvider>
    $<TARGET_FILE:CCQuickStartupExtension>
)
list(JOIN EXTENSION_TARGETS ";" ANDROID_EXTRA_LIBS)

get_target_property(_extra_libs OrangeLive QT_ANDROID_EXTRA_LIBS)
list(APPEND ANDROID_EXTRA_LIBS ${_extra_libs})

set_target_properties(OrangeLive PROPERTIES
    QT_ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/android
    QT_ANDROID_EXTRA_LIBS "${ANDROID_EXTRA_LIBS}"
    QT_ANDROID_EXTRA_PLUGINS "${CMAKE_BINARY_DIR}/Extension/Modules/"
)

target_link_libraries(OrangeLive PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Concurrent
    Qt6::Sql
    Qt6::Network
    CCQuickStartup
)

target_compile_definitions(OrangeLive PRIVATE ORANGE_ARCH_STRING="${ANDROID_ABI}")
