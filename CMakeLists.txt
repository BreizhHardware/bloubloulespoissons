cmake_minimum_required(VERSION 3.18)
project(bloubloulespoissons)

# Configurer le compilateur pour la Wii U
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR powerpc)
set(DEVKITPRO $ENV{DEVKITPRO})
set(DEVKITPPC $ENV{DEVKITPPC})

set(CMAKE_PREFIX_PATH "/opt/devkitpro/portlibs/wiiu")
include_directories(/opt/devkitpro/portlibs/wiiu/include/SDL2)
link_directories(/opt/devkitpro/portlibs/wiiu/lib)

# Configurer les bibliothèques SDL2 pour Wii U
include_directories(
        ${DEVKITPRO}/portlibs/ppc/include
        ${DEVKITPRO}/libogc/include
)
link_directories(
        ${DEVKITPRO}/portlibs/ppc/lib
        ${DEVKITPRO}/libogc/lib/wiiu
)

# Spécifier le standard C++ à utiliser
set(CMAKE_CXX_STANDARD 23)

# Définir les fichiers sources et d'en-têtes
add_executable(bloubloulespoissons
        main.cpp
        fish.cpp decors.cpp
        camera.cpp camera.h
        env.cpp env.h
        player.cpp player.h
        menu.cpp menu.h
        display.cpp display.h
        shark.cpp shark.h
)

# Liens avec les bibliothèques Wii U
target_link_libraries(bloubloulespoissons
        sdl2
        sdl2_image
        sdl2_ttf
        sdl2_mixer
        png
        ogc
        vorbisidec
        m
)

# Options de compilation
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -g")

# Générer un fichier RPX si nécessaire
set_target_properties(bloubloulespoissons PROPERTIES OUTPUT_NAME "bloubloulespoissons.elf")
