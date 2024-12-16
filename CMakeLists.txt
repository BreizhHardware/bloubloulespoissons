cmake_minimum_required(VERSION 3.18)

# Définir les chemins vers DEVKITPRO et DEVKITPPC
set(DEVKITPRO "/opt/devkitpro")
set(DEVKITPPC "/opt/devkitpro/devkitPPC")

# Configuration du projet
project(bloubloulespoissons)

# Définir les chemins vers les bibliothèques Wii U
set(CMAKE_PREFIX_PATH "${DEVKITPRO}/portlibs/wiiu" "${DEVKITPRO}/portlibs/ppc")
include_directories(
        "${DEVKITPRO}/portlibs/wiiu/include"
        "${DEVKITPRO}/portlibs/wiiu/include/SDL2"
)
link_directories(
        "${DEVKITPRO}/portlibs/wiiu/lib"
        "${DEVKITPRO}/portlibs/ppc/lib"
)

# Spécifier la norme C++ utilisée
set(CMAKE_CXX_STANDARD 23)

# Définir les flags de compilation pour PowerPC
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")

# Définir explicitement le compilateur
set(CMAKE_C_COMPILER "${DEVKITPPC}/bin/powerpc-eabi-gcc")
set(CMAKE_CXX_COMPILER "${DEVKITPPC}/bin/powerpc-eabi-g++")

# Trouver SDL2 manuellement
set(SDL2_LIBRARIES "${DEVKITPRO}/portlibs/wiiu/lib/libSDL2.a")
set(SDL2_INCLUDE_DIRS "${DEVKITPRO}/portlibs/wiiu/include/SDL2")

# Définir les fichiers source du projet
add_executable(bloubloulespoissons
        main.cpp
        fish.cpp
        decors.cpp
        camera.cpp camera.h
        env.cpp env.h
        player.cpp player.h
        menu.cpp menu.h
        display.cpp display.h
        shark.cpp shark.h
)

# Lier les bibliothèques SDL2 et autres nécessaires
target_include_directories(bloubloulespoissons PUBLIC ${SDL2_INCLUDE_DIRS})
target_link_libraries(bloubloulespoissons
        ${SDL2_LIBRARIES}
        "${DEVKITPRO}/portlibs/ppc/lib/libogg.a"
        "${DEVKITPRO}/portlibs/ppc/lib/libvorbisenc.a"
        "${DEVKITPRO}/portlibs/ppc/lib/libpng.a"
        m
        wiiload
        GX2
        procui
)

# Options de compilation
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -g")

# Générer le fichier ELF
set_target_properties(bloubloulespoissons PROPERTIES OUTPUT_NAME "bloubloulespoissons.elf")

# Optionnel: Spécifier une sortie dans un répertoire spécifique (par exemple, 'output')
# install(TARGETS bloubloulespoissons DESTINATION output)
