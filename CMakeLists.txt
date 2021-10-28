cmake_minimum_required(VERSION 3.14)

project(FTGL LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#include_directories(include)
include_directories(src)

add_library(
        glyphy SHARED

        # FT Font
        src/glyphy-arc.cc
        src/glyphy-arcs.cc
        src/glyphy-blob.cc
        src/glyphy-extents.cc
        src/glyphy-outline.cc
        src/glyphy-sdf.cc
        src/glyphy-shaders.cc
)

target_link_libraries(
        glyphy
        freetype-cmake
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::OpenGL
)
