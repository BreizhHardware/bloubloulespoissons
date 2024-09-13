cmake_minimum_required(VERSION 3.24)
project(bloubloulespoissons)

set(CMAKE_CXX_STANDARD 17)

# Ajouter le chemin de la bibliothèque SDL2
set(CMAKE_PREFIX_PATH "C:/mingw_dev_lib")

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(bloubloulespoissons main.cpp)

# Lier SDL2
target_link_libraries(bloubloulespoissons ${SDL2_LIBRARIES})