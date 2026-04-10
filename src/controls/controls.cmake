set(_VIDEO_SDK_LIBS avutil avformat avfilter avcodec swscale swresample avdevice postproc mpv)
foreach(name ${_VIDEO_SDK_LIBS})
    find_library(_LIB_${name}
        NAMES ${name}
        PATHS ${CMAKE_SOURCE_DIR}/third_party/libmpv/libs/${ANDROID_ABI}
        NO_DEFAULT_PATH
        NO_CMAKE_FIND_ROOT_PATH
    )
    if(_LIB_${name})
        list(APPEND _VIDEO_DEPENDS_SDK ${_LIB_${name}})
    endif()
endforeach()

function(add_control_android_dependencies)
    foreach(TARGET_NAME ${ARGN})
        if(TARGET ${TARGET_NAME})
            set_property(TARGET ${TARGET_NAME} APPEND PROPERTY
                QT_ANDROID_EXTRA_LIBS ${_VIDEO_DEPENDS_SDK}
            )
        else()
            message(WARNING "add_control_android_dependencies(): target '${TARGET_NAME}' does not exist.")
        endif()
    endforeach()
endfunction()
