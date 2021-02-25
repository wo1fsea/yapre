cmake_minimum_required(VERSION 3.10)
project(yapre)

set(CMAKE_CXX_STANDARD 17)

include_directories(
    include/
    )

file(GLOB PROJECT_INCLUDE include/*.hpp
                          include/*.h
                          )
file(GLOB PROJECT_SRC src/*.cpp)
file(GLOB PROJECT_CONFIG CMakeLists.txt
                          Readme.md
                         .gitattributes
                         .gitignore
                         .gitmodules)

source_group("include" FILES ${PROJECT_INCLUDE})
source_group("src" FILES ${PROJECT_SRC})

add_executable(${PROJECT_NAME} ${PROJECT_INCLUDE} ${PROJECT_SRC})

set_target_properties(
    ${PROJECT_NAME} 
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME}
    )

add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMAND cp ./platform/emscripten/index.html ${CMAKE_BINARY_DIR}/${PROJECT_NAME}
    )
