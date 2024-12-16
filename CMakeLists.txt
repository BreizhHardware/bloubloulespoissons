cmake_minimum_required(VERSION 3.18)

# Define DEVKITPRO and DEVKITPPC paths
set(DEVKITPRO "/opt/devkitpro")
set(DEVKITPPC "/opt/devkitpro/devkitPPC")

# Project configuration
project(bloubloulespoissons)

# Define paths for Wii U
set(CMAKE_PREFIX_PATH "${DEVKITPRO}/portlibs/wiiu" "${DEVKITPRO}/portlibs/ppc")
include_directories("${DEVKITPRO}/portlibs/wiiu/include" "${DEVKITPRO}/portlibs/wiiu/include/SDL2")
link_directories("${DEVKITPRO}/portlibs/wiiu/lib" "${DEVKITPRO}/portlibs/ppc/lib")

# Specify C++ standard
set(CMAKE_CXX_STANDARD 23)

# Set PowerPC compilation flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32 ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32 ")

# Find SDL2 manually
set(SDL2_LIBRARIES "${DEVKITPRO}/portlibs/wiiu/lib/libSDL2.a")
set(SDL2_INCLUDE_DIRS "${DEVKITPRO}/portlibs/wiiu/include/SDL2")

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
target_include_directories(bloubloulespoissons PUBLIC ${SDL2_INCLUDE_DIRS})
target_link_libraries(bloubloulespoissons
        ${SDL2_LIBRARIES}
        "${DEVKITPRO}/portlibs/ppc/lib/libogg.a"
        "${DEVKITPRO}/portlibs/ppc/lib/libvorbisenc.a"
        "${DEVKITPRO}/portlibs/ppc/lib/libpng.a"
        m
)

# Compilation options
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -g")

# Generate RPX file if needed
set_target_properties(bloubloulespoissons PROPERTIES OUTPUT_NAME "bloubloulespoissons.elf")