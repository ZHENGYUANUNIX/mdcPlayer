cmake_minimum_required(VERSION 3.22)

project(mdcPlayer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME} main.cpp
    msgDef.h msgDef.cpp
    player.h player.cpp
)
link_libraries(${PROJECT_NAME} pthread)
