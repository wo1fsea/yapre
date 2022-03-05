cmake_minimum_required(VERSION 3.10)

function(platform_setup_target)
    set_target_properties(
        ${PROJECT_NAME} 
        PROPERTIES
        # LINK_FLAGS " --preload-file ./data -O2 -g4 -s ASSERTIONS=1 -s USE_SDL=2 -s FULL_ES2=1  -s ALLOW_MEMORY_GROWTH=1"
        LINK_FLAGS "--preload-file ./data -gsource-map -s MAX_WEBGL_VERSION=2 -s ALLOW_MEMORY_GROWTH=1 -s INITIAL_MEMORY=209715200 -s MAXIMUM_MEMORY=4GB -s USE_PTHREADS=1 -s PTHREAD_POOL_SIZE=8 -Wl,--shared-memory,--no-check-features -s DEFAULT_LIBRARY_FUNCS_TO_INCLUDE=[\"$autoResumeAudioContext\"] -s DYNCALLS=1"
        )
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")
    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMAND cp ./platform/emscripten/index.html ./platform/emscripten/favicon.ico ${CMAKE_BINARY_DIR}/${PROJECT_NAME}
        )
endfunction()
