cmake_minimum_required(VERSION 3.5)
project(gol)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
SET(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -static")

file(GLOB_RECURSE SOURCE_GOL src/*.cpp)

include_directories(raylib)

add_executable(gol ${SOURCE_GOL})

add_subdirectory(raylib)

target_link_libraries(gol raylib)
