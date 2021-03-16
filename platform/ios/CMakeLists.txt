cmake_minimum_required(VERSION 3.10)

# set(CODE_SIGN_IDENTITY "Apple Development: quanyong huang (634ST6FXWK)")

set(MACOSX_BUNDLE_EXECUTABLE_NAME ${PROJECT_NAME})
set(MACOSX_BUNDLE_INFO_STRING  ${PROJECT_NAME}})
set(MACOSX_BUNDLE_GUI_IDENTIFIER  ${PROJECT_NAME})
set(MACOSX_BUNDLE_BUNDLE_NAME  ${PROJECT_NAME})
set(MACOSX_BUNDLE_ICON_FILE "")
set(MACOSX_BUNDLE_LONG_VERSION_STRING "1.0")
set(MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0")
set(MACOSX_BUNDLE_BUNDLE_VERSION "1.0")
set(MACOSX_BUNDLE_COPYRIGHT "Copyright yapre")
# set(MACOSX_DEPLOYMENT_TARGET ${DEPLOYMENT_TARGET})


function(platform_setup_target)
    target_link_libraries(
        ${PROJECT_NAME} 
        "-framework CoreAudio" 
        "-framework AudioToolbox"
        "-framework CoreFoundation"
        "-framework CoreGraphics"
        "-framework CoreVideo"
        # "-framework ForceFeedback"
        "-framework IOKit"
        # "-framework CarBon"
        # "-framework AppKit"
        "-framework CoreHaptics"
        iconv
        )
    set_target_properties(
        ${PROJECT_NAME} PROPERTIES
        # XCODE_ATTRIBUTE_DEBUG_INFORMATION_FORMAT "dwarf-with-dsym"
        # XCODE_ATTRIBUTE_GCC_PREFIX_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/Prefix.pch"
        # XCODE_ATTRIBUTE_GCC_PRECOMPILE_PREFIX_HEADER "YES"
        # XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET ${DEPLOYMENT_TARGET}
        # XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "${CODE_SIGN_IDENTITY}"
        # XCODE_ATTRIBUTE_DEVELOPMENT_TEAM ${DEVELOPMENT_TEAM_ID}
        # XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY ${DEVICE_FAMILY}
        # MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/plist.in
        # XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES
        # XCODE_ATTRIBUTE_COMBINE_HIDPI_IMAGES NO
        # XCODE_ATTRIBUTE_INSTALL_PATH "$(LOCAL_APPS_DIR)"
        # XCODE_ATTRIBUTE_ENABLE_TESTABILITY YES
        # XCODE_ATTRIBUTE_GCC_SYMBOLS_PRIVATE_EXTERN YES
        RESOURCE "${PROJECT_RESOURCE}"
        )
endfunction()
