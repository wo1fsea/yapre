cmake_minimum_required(VERSION 3.10)
project(yapre)
set(CMAKE_CXX_STANDARD 17)

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


if(YAPRE_BUILD_PLATFORM STREQUAL "emscripten")
else()
    target_link_libraries(${PROJECT_NAME} SDL2main SDL2-static)
endif()

set_target_properties(
    ${PROJECT_NAME} 
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME}
    )

platform_setup_target()

unset(YAPRE_BUILD_PLATFORM CACHE)
