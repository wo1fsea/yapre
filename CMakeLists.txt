cmake_minimum_required(VERSION 3.10)

project(yapre)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(BUILD_SHARED_LIBS OFF)
set(BUILD_STATIC_LIBS ON)

function(platform_setup_target)
endfunction()

option(YAPRE_BUILD_PLATFORM "Platform" "") 
if(YAPRE_BUILD_PLATFORM STREQUAL "emscripten")
    include(platform/emscripten/CMakeLists.txt)
endif()
if(YAPRE_BUILD_PLATFORM STREQUAL "ios")
    include(platform/ios/CMakeLists.txt)
endif()

remove_definitions(-Werror)
option(SDL_SHARED FALSE)
set(SDL2_DIR "external/SDL")
add_subdirectory(${SDL2_DIR})

set(GLM_DIR "external/glm")
add_subdirectory(${GLM_DIR})

set(STB_IMAGE_DIR "external/std_image")
add_subdirectory(${STB_IMAGE_DIR})

set(LUA_DIR "external/lua")
add_subdirectory(${LUA_DIR})

set(CPP_LONENOISE_DIR "external/cpp-linenoise")
add_subdirectory(${CPP_LONENOISE_DIR})

set(FREETYPE_DIR "external/freetype")
add_subdirectory(${FREETYPE_DIR})

set(UTFCPP_DIR "external/utfcpp")
add_subdirectory(${UTFCPP_DIR})

set(BOX2D_DIR "external/box2d")
option(BOX2D_BUILD_UNIT_TESTS OFF)
option(BOX2D_BUILD_TESTBED OFF)
add_subdirectory(${BOX2D_DIR})

set(ASIO_DIR "external/asio")
option(ASIO_NO_DEPRECATED ON)

set(BGFX_CMAKE_DIR "external/bgfx.cmake")
option(BGFX_BUILD_TOOLS ON)
option(BGFX_BUILD_EXAMPLES OFF)
option(BGFX_INSTALL OFF)
option(BGFX_INSTALL_EXAMPLES OFF)
option(BGFX_CUSTOM_TARGETS OFF)
option(BGFX_AMALGAMATED OFF)
option(BX_AMALGAMATED OFF)
option(BGFX_CONFIG_RENDERER_WEBGPU OFF)
add_subdirectory(${BGFX_CMAKE_DIR})

# option(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
# option(ASSIMP_BUILD_TESTS OFF)

# set(ASSIMP_DIR "external/assimp")
# set(ASSIMP_INCLUDE_DIRS ${ASSIMP_DIR}/include "build/emscripten/external/assimp/include")
# add_subdirectory(${ASSIMP_DIR})

include_directories(
    src/
    src/systems/
    src/systems/graphics/
    ${SDL2_DIR}/include
    ${SDL2_DIR}/src
    ${ASSIMP_INCLUDE_DIRS} 
    ${GLM_DIR}
    ${STB_IMAGE_DIR}/include
    ${LUA_DIR}/src
    ${CPP_LONENOISE_DIR}
    ${FREETYPE_DIR}/include
    ${UTFCPP_DIR}/source
    ${ASIO_DIR}/asio/include
    ${BOX2D_DIR}/include
    )

file(GLOB_RECURSE PROJECT_INCLUDE 
    src/*.h
    src/*.hpp
    src/*.inl
    src/*/*.h
    src/*/*.hpp
    src/*/*.inl
    )
file(GLOB_RECURSE PROJECT_SRC 
    src/*.cpp
    src/*.mm
    src/*/*.cpp
    src/*/*.mm
    )

file(GLOB PROJECT_CONFIG 
    CMakeLists.txt
    Readme.md
    .gitattributes
    .gitignore
    .gitmodules
    )
file(GLOB PROJECT_RESOURCE
    data
    )

source_group("include" FILES ${PROJECT_INCLUDE})
source_group("src" FILES ${PROJECT_SRC})

if(YAPRE_BUILD_PLATFORM STREQUAL "windows")
    add_compile_options(-DYAPRE_WINDOWS)
elseif(YAPRE_BUILD_PLATFORM STREQUAL "mac")
    add_compile_options(-DYAPRE_MAC)
elseif(YAPRE_BUILD_PLATFORM STREQUAL "linux")
    add_compile_options(-DYAPRE_LINUX)
elseif(YAPRE_BUILD_PLATFORM STREQUAL "emscripten")
    add_compile_options(-DYAPRE_EMSCRIPTEN)
elseif(YAPRE_BUILD_PLATFORM STREQUAL "android")
    add_compile_options(-DYAPRE_ANDROID)
elseif(YAPRE_BUILD_PLATFORM STREQUAL "ios")
    add_compile_options(-DYAPRE_IOS)
endif()

add_executable(
    ${PROJECT_NAME} 
    ${PROJECT_INCLUDE} 
    ${PROJECT_SRC}
    ${PROJECT_RESOURCE}
    )

target_link_libraries(
    ${PROJECT_NAME} 
    SDL2main 
    SDL2-static 
    glm_static
    std_image
    lua
    freetype
    box2d
    bgfx
    bx
    bimg
    # assimp 
    )

set_target_properties(
    ${PROJECT_NAME} 
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME}
    )

if(NOT YAPRE_BUILD_PLATFORM STREQUAL "windows")
add_custom_command(
    TARGET ${PROJECT_NAME}
    PRE_BUILD
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMAND cp -r ./data ${CMAKE_BINARY_DIR}
    )
endif()

platform_setup_target()

#if(YAPRE_BUILD_PLATFORM STREQUAL "windows")
#   add_custom_command(
#       TARGET ${PROJECT_NAME}
#       POST_BUILD
#       WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
#       COMMAND set "p1=${CMAKE_BINARY_DIR}/compile_commands.json"
#       COMMAND set "p2=${PROJECT_SOURCE_DIR}"
#       COMMAND set "p1=%p1:/=\\%"
#       COMMAND set "p2=%p2:/=\\%"
#       COMMAND copy "%p1%" "%p2%"
#       )
#endif()

if(YAPRE_BUILD_PLATFORM STREQUAL "mac" OR YAPRE_BUILD_PLATFORM STREQUAL "emscripten")
    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMAND cp ${CMAKE_BINARY_DIR}/compile_commands.json ${PROJECT_SOURCE_DIR}/
        )
endif()

