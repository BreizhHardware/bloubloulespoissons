cmake_minimum_required(VERSION 3.24)
project(bloubloulespoissons)

set(CMAKE_CXX_STANDARD 17)

# Trouver SDL2 version 2.26.5
find_package(SDL2 2.26.5 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(bloubloulespoissons main.cpp)

# Lier SDL2
target_link_libraries(bloubloulespoissons ${SDL2_LIBRARIES})