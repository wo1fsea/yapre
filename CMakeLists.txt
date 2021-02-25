cmake_minimum_required(VERSION 3.10)
project(yapre)

set(CMAKE_CXX_STANDARD 17)

# set(SDL2_DIR "external/SDL")
# add_subdirectory(${SDL2_DIR})

include_directories(
    include/
    ${SDL2_DIR}/include
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
# target_link_libraries(${PROJECT_NAME} SDL2main SDL2-static)

set_target_properties(
    ${PROJECT_NAME} 
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME}
    LINK_FLAGS "-O2 -s USE_SDL=2 -s USE_WEBGL2=1"
    )

include(platform/emscripten/CMakeLists.txt)
