cmake_minimum_required(VERSION 3.22)
project(mdcPlayer LANGUAGES CXX)

set(CMAKE_C_COMPILER "/usr/local/mdc_sdk/dp_gea/mdc_cross_compiler/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/local/mdc_sdk/dp_gea/mdc_cross_compiler/bin/clang++")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
FILE(GLOB SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_executable(${PROJECT_NAME} ${SRC_FILES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} PRIVATE pthread)
