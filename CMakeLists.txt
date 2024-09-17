cmake_minimum_required(VERSION 3.18)
project(bloubloulespoissons)

set(CMAKE_CXX_STANDARD 17)

# Ajouter le chemin de la bibliothèque SDL2
set(CMAKE_PREFIX_PATH "C:/SDL2")

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})

include_directories("C:/Program Files (x86)/Visual Leak Detector/include")

# Ajouter le chemin de la bibliothèque pour VLD
link_directories("C:/Program Files (x86)/Visual Leak Detector/lib/Win64")

add_executable(bloubloulespoissons main.cpp fish.cpp decors.cpp)

# Lier SDL2 et SDL2_image
target_link_libraries(bloubloulespoissons ${SDL2_LIBRARIES} -lSDL2_image vld)
