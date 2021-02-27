cmake_minimum_required(VERSION 3.10)

function(platform_setup_target)
    set_target_properties(
        ${PROJECT_NAME} 
        PROPERTIES
        LINK_FLAGS "-O2 -s USE_SDL=2 -s USE_WEBGL2=1"
        )
    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMAND cp ./platform/emscripten/index.html ${CMAKE_BINARY_DIR}/${PROJECT_NAME}
        )
endfunction()
