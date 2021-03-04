cmake_minimum_required(VERSION 3.10)
project(yapre)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(BUILD_SHARED_LIBS OFF)
set(BUILD_STATIC_LIBS ON)

function(platform_setup_target)
endfunction()

option(YAPRE_BUILD_PLATFORM "Platform" "") #"" by default
if(YAPRE_BUILD_PLATFORM STREQUAL "emscripten")
    include(platform/emscripten/CMakeLists.txt)
else()
    option(SDL_SHARED FALSE)
    set(SDL2_DIR "external/SDL")
    add_subdirectory(${SDL2_DIR})
    if(YAPRE_BUILD_PLATFORM STREQUAL "ios")
        include(platform/ios/CMakeLists.txt)
    endif()
endif()

set(GLAD_DIR "external/glad")
add_subdirectory(${GLAD_DIR})

set(GLM_DIR "external/glm")
add_subdirectory(${GLM_DIR})

option(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
option(ASSIMP_BUILD_TESTS OFF)

set(ASSIMP_DIR "external/assimp")
set(ASSIMP_INCLUDE_DIRS ${ASSIMP_DIR}/include "build/emscripten/external/assimp/include")
add_subdirectory(${ASSIMP_DIR})

include_directories(
    include/
    ${SDL2_DIR}/include
    ${GLAD_DIR}/include
    ${ASSIMP_INCLUDE_DIRS} 
    ${GLM_DIR}
    )

file(GLOB PROJECT_INCLUDE 
    include/*.hpp 
    include/*.h
    )
file(GLOB PROJECT_SRC 
    src/*.cpp
    )
file(GLOB PROJECT_CONFIG 
    CMakeLists.txt
    Readme.md
    .gitattributes
    .gitignore
    .gitmodules
    )

source_group("include" FILES ${PROJECT_INCLUDE})
source_group("src" FILES ${PROJECT_SRC})

add_executable(${PROJECT_NAME} ${PROJECT_INCLUDE} ${PROJECT_SRC})


if(YAPRE_BUILD_PLATFORM STREQUAL "emscripten")
    target_link_libraries(${PROJECT_NAME} glad assimp glm_static)
else()
    target_link_libraries(${PROJECT_NAME} SDL2main SDL2-static glad assimp glm_static)
endif()

set_target_properties(
    ${PROJECT_NAME} 
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME}
    )

platform_setup_target()

add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMAND cp ${CMAKE_BINARY_DIR}/compile_commands.json ${PROJECT_SOURCE_DIR}/
    )
