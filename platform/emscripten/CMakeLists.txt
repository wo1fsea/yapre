cmake_minimum_required(VERSION 3.10)

function(platform_setup_target)
    set_target_properties(
        ${PROJECT_NAME} 
        PROPERTIES
        LINK_FLAGS "-O2 -g4 -s USE_SDL=2 -s FULL_ES3=1 --preload-file ./data -s ASSERTIONS=1 -s ALLOW_MEMORY_GROWTH=1"
        )
    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMAND cp ./platform/emscripten/index.html ${CMAKE_BINARY_DIR}/${PROJECT_NAME}
        )
    add_custom_command(
        TARGET ${PROJECT_NAME}
        PRE_BUILD
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMAND cp -r ./data ${CMAKE_BINARY_DIR}
        )

endfunction()
