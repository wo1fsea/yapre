cmake_minimum_required(VERSION 3.10)
project(lua)
set(CMAKE_CXX_STANDARD 17)

file(GLOB PROJECT_INCLUDE 
    src/*.hpp 
    src/*.h
    )
file(GLOB PROJECT_SRC 
    src/*.cpp 
    src/*.c
    )
file(GLOB PROJECT_CONFIG 
    CMakeLists.txt
    Readme.md
    .gitattributes
    .gitignore
    .gitmodules
    )

list(REMOVE_ITEM lib_srcs "src/lua.c")

include_directories(
    src/
    )

source_group("include" FILES ${PROJECT_INCLUDE})
source_group("src" FILES ${PROJECT_SRC})

ADD_LIBRARY(${PROJECT_NAME} STATIC ${PROJECT_SRC})
