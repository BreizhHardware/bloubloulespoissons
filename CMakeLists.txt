cmake_minimum_required(VERSION 3.18)
project(bloubloulespoissons)

# Configurer le compilateur pour la Wii U
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR powerpc)
set(DEVKITPRO $ENV{DEVKITPRO})
set(DEVKITPPC $ENV{DEVKITPPC})
set(CMAKE_TOOLCHAIN_FILE "$ENV{DEVKITPRO}/cmake/devkitPPC.cmake")

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
