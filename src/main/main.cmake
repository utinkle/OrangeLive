qt_add_executable(OrangeLive WIN32 MACOSX_BUNDLE
    ${CMAKE_CURRENT_LIST_DIR}/application.qrc
    ${CMAKE_CURRENT_LIST_DIR}/main.cpp
)

set(EXTENSION_TARGETS
    $<TARGET_FILE:OrangeLiveCorePreload>
    $<TARGET_FILE:OrangeLiveCoreEntity>
    $<TARGET_FILE:OrangeLiveControls>
    $<TARGET_FILE:OrangeLiveProvider>
    $<TARGET_FILE:CCQuickStartupExtension>
)
list(JOIN EXTENSION_TARGETS ";" ANDROID_EXTRA_LIBS)

set_target_properties(OrangeLive PROPERTIES
    QT_ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/android
    QT_ANDROID_EXTRA_LIBS "${ANDROID_EXTRA_LIBS}"
)

target_link_libraries(OrangeLive PRIVATE
    Qt::Core
    Qt::Gui
    Qt::Quick
    Qt::QuickControls2
    CCQuickStartup
)

target_compile_definitions(OrangeLive PRIVATE ORANGE_ARCH_STRING="${CMAKE_SYSTEM_PROCESSOR}")
