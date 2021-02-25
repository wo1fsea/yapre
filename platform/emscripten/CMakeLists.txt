cmake_minimum_required(VERSION 3.10)

add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMAND cp ./platform/emscripten/index.html ${CMAKE_BINARY_DIR}/${PROJECT_NAME}
    )
