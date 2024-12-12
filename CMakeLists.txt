cmake_minimum_required(VERSION 3.18)
project(bloubloulespoissons)

set(CMAKE_CXX_STANDARD 23)

# Ajouter le chemin de la bibliothèque SDL2
if (WIN32)
    set(CMAKE_PREFIX_PATH "C:/SDL2")
else()
    set(CMAKE_PREFIX_PATH "/usr/local/lib/cmake/SDL2")
endif()

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_net REQUIRED)
find_package(SDL2_mixer REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS} ${SDL2_NET_INCLUDE_DIRS})

add_executable(bloubloulespoissons main.cpp fish.cpp decors.cpp
        camera.cpp
        camera.h
        env.h
        env.cpp
        player.h
        player.cpp
        menu.h
        menu.cpp
        network/networking.cpp
        network/networking.h
        network/networking_client.cpp
        network/networking_client.h
        display.cpp
        display.h
        shark.cpp
        shark.h
)

# Lier SDL2 et SDL2_image
target_link_libraries(bloubloulespoissons ${SDL2_LIBRARIES} SDL2::SDL2 SDL2_image::SDL2_image SDL2_ttf::SDL2_ttf SDL2_net::SDL2_net SDL2_mixer::SDL2_mixer)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")

if (WIN32)
    if (MINGW)
        set_target_properties(bloubloulespoissons PROPERTIES LINK_FLAGS "-Wl,-subsystem,console")
    else()
        set_target_properties(bloubloulespoissons PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")
    endif()
    target_link_libraries(bloubloulespoissons ws2_32)
endif()

# Options pour la liaison statique en mode release
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
endif()