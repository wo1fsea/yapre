cmake_minimum_required(VERSION 3.10)
project(yapre)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(BUILD_SHARED_LIBS OFF)
set(BUILD_STATIC_LIBS ON)

function(platform_setup_target)
endfunction()

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
option(BGFX_BUILD_TOOLS OFF)
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
    src/*/*.cpp
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

add_compile_options(-DYAPRE_ANDROID)
add_library(
        ${PROJECT_NAME}
        SHARED
        ${PROJECT_INCLUDE}
        ${PROJECT_SRC}
)

message(${PROJECT_INCLUDE})

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

platform_setup_target()
