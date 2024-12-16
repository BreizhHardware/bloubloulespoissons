cmake_minimum_required(VERSION 3.18)

# Define DEVKITPRO and DEVKITPPC paths
set(DEVKITPRO "C:/devkitpro")
set(DEVKITPPC "C:/devkitpro/devkitPPC")

# Project configuration
project(bloubloulespoissons)

# Define paths for Wii U
set(CMAKE_PREFIX_PATH "${DEVKITPRO}/portlibs/wiiu" "${DEVKITPRO}/portlibs/ppc" "${DEVKITPRO}/portlibs/wiiu/lib/cmake/SDL2")

include_directories("${DEVKITPRO}/portlibs/wiiu/include" "${DEVKITPRO}/portlibs/wiiu/include/SDL2" "${DEVKITPRO}/portlibs/ppc/include")
link_directories("${DEVKITPRO}/portlibs/wiiu/lib" "${DEVKITPRO}/portlibs/ppc/lib")

# Specify C++ standard
set(CMAKE_CXX_STANDARD 23)

# Set PowerPC compilation flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32 -mcpu=750 -meabi -mhard-float")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32 -mcpu=750 -meabi -mhard-float")

# Find SDL2 packages
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_net REQUIRED)
find_package(SDL2_mixer REQUIRED)

# Define source files
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

# Link with SDL2 libraries
target_link_libraries(bloubloulespoissons
        SDL2::SDL2
        SDL2_image::SDL2_image
        SDL2_ttf::SDL2_ttf
        SDL2_net::SDL2_net
        SDL2_mixer::SDL2_mixer
        "${DEVKITPRO}/portlibs/ppc/lib/libogg.la"
        "${DEVKITPRO}/portlibs/ppc/lib/libvorbisenc.la"
        "${DEVKITPRO}/portlibs/ppc/lib/libpng.la"
        m  # Add the math library
)

# Compilation options
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -g")

# Generate RPX file if needed
set_target_properties(bloubloulespoissons PROPERTIES OUTPUT_NAME "bloubloulespoissons.elf")