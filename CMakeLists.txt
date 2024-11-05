cmake_minimum_required(VERSION 3.18)
project(bloubloulespoissons)

set(CMAKE_CXX_STANDARD 17)

# Ajouter le chemin de la bibliothèque SDL2
set(CMAKE_PREFIX_PATH "C:/SDL2")

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS})

add_executable(bloubloulespoissons main.cpp fish.cpp decors.cpp
        camera.cpp
        camera.h)

# Lier SDL2 et SDL2_image
target_link_libraries(bloubloulespoissons ${SDL2_LIBRARIES} -lSDL2_image -lSDL2_ttf)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")

if (WIN32)
    if (MINGW)
        set_target_properties(bloubloulespoissons PROPERTIES LINK_FLAGS "-Wl,-subsystem,console")
    else()
        set_target_properties(bloubloulespoissons PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")
    endif()
endif()