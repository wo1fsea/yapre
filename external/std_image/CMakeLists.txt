cmake_minimum_required(VERSION 3.10)
project(std_image)
set(CMAKE_CXX_STANDARD 17)

file(GLOB PROJECT_INCLUDE 
    include/*.hpp 
    include/*.h
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

include_directories(
    include/
    )

source_group("include" FILES ${PROJECT_INCLUDE})
source_group("src" FILES ${PROJECT_SRC})

ADD_LIBRARY(${PROJECT_NAME} STATIC ${PROJECT_SRC})
