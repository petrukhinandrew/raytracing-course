cmake_minimum_required(VERSION 3.10)

project(raytracing)

# replace with gcc
set(CMAKE_C_COMPILER "clang")
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_SOURCE_DIR}/src)

add_executable(${PROJECT_NAME} src/main.cpp)
