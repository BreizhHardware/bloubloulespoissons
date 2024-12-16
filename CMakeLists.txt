cmake_minimum_required(VERSION 3.18)

# Définir les chemins de DEVKITPRO et DEVKITPPC
set(DEVKITPRO "/opt/devkitpro")
set(DEVKITPPC "/opt/devkitpro/devkitPPC")

# Configurer le projet et les chemins de compilation
project(bloubloulespoissons)

# Définir les chemins pour Wii U
set(CMAKE_PREFIX_PATH "${DEVKITPRO}/portlibs/wiiu")
include_directories(${DEVKITPRO}/portlibs/wiiu/include/SDL2)
link_directories(${DEVKITPRO}/portlibs/wiiu/lib)

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
        SDL2
        SDL2_image
        SDL2_ttf
        SDL2_mixer
        ogg
        vorbisidec
        png
)

# Options de compilation
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -g")

# Générer un fichier RPX si nécessaire
set_target_properties(bloubloulespoissons PROPERTIES OUTPUT_NAME "bloubloulespoissons.elf")