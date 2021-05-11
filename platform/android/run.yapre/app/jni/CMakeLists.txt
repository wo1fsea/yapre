cmake_minimum_required(VERSION 3.10)
project(yapre)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(BUILD_SHARED_LIBS OFF)
set(BUILD_STATIC_LIBS ON)

function(platform_setup_target)
endfunction()

option(SDL_SHARED OFF)
set(SDL2_DIR "external/SDL")
add_subdirectory(${SDL2_DIR})

set(GLAD_DIR "external/glad")
add_subdirectory(${GLAD_DIR})

set(GLM_DIR "external/glm")
add_subdirectory(${GLM_DIR})

set(STB_IMAGE_DIR "external/std_image")
add_subdirectory(${STB_IMAGE_DIR})

set(LUA_DIR "external/lua")
add_subdirectory(${LUA_DIR})

set(CPP_LONENOISE_DIR "external/cpp-linenoise")
add_subdirectory(${CPP_LONENOISE_DIR})

# option(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
# option(ASSIMP_BUILD_TESTS OFF)

# set(ASSIMP_DIR "external/assimp")
# set(ASSIMP_INCLUDE_DIRS ${ASSIMP_DIR}/include "build/emscripten/external/assimp/include")
# add_subdirectory(${ASSIMP_DIR})

include_directories(
        ./src/
        ${SDL2_DIR}/include
        ${GLAD_DIR}/include
        ${ASSIMP_INCLUDE_DIRS}
        ${GLM_DIR}
        ${STB_IMAGE_DIR}/include
        ${LUA_DIR}/src
        ${CPP_LONENOISE_DIR}
)

file(GLOB PROJECT_INCLUDE
        ./src/*.hpp
        ./src/*.h
        )
file(GLOB PROJECT_SRC
        ./src/*.cpp
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
        glad
        glm_static
        std_image
        lua
        # assimp
)


platform_setup_target()



