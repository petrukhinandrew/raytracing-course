cmake_minimum_required(VERSION 3.10)

project(raytracing)

# replace with gcc
set(CMAKE_C_COMPILER "clang")
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CONFIGURATION_TYPES "Debug" CACHE STRING "" FORCE)

include_directories(${CMAKE_SOURCE_DIR}/src)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/intersection/Intersection.h
    src/lights/LightSource.h
    src/lights/LightCommandFactory.h
    src/image_writer/ImageWriter.h
    src/image_writer/NetPbmImageWriter.h
    src/parser/Command.h
    src/parser/FileParser.h
    src/camera/Camera.h
    src/scene/Scene.h
    src/scene/ImageScene.h
    src/ray/Ray.h
    src/primitives/Plane.h
    src/primitives/Ellipsoid.h
    src/primitives/Box.h
    src/primitives/Primitive.h
    src/primitives/Material.h
    src/primitives/Properties.h
    src/primitives/PrimitiveCommandFactory.h
)

target_include_directories(
    ${PROJECT_NAME} PUBLIC glm
)
