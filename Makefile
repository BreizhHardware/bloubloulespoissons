# Chemins des outils devkitPro et devkitPPC
DEVKITPRO ?= /opt/devkitpro
DEVKITPPC ?= $(DEVKITPRO)/devkitPPC

# Chemins des bibliothèques et des fichiers d'inclusion
INCLUDES := -I$(DEVKITPRO)/portlibs/wiiu/include \
            -I$(DEVKITPRO)/portlibs/wiiu/include/SDL2 \
            -I$(DEVKITPRO)/portlibs/wiiu/include \
            -I$(DEVKITPRO)/portlibs/ppc/include \
            -I$(DEVKITPRO)/portlibs/ppc/include/freetype2 \
            -I$(DEVKITPRO)/portlibs/ppc/include/harfbuzz

LIBS := -L$(DEVKITPRO)/portlibs/wiiu/lib \
        -L$(DEVKITPRO)/portlibs/ppc/lib \
        -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lm -lwut \
        -lfreetype -lharfbuzz

# Compilateurs
CXX := $(DEVKITPPC)/bin/powerpc-eabi-g++

# Options de compilation
CXXFLAGS := -O2 -g -std=c++23 -m32 -Wall

# Fichiers sources et objets
SRCS := main.cpp camera.cpp decors.cpp display.cpp env.cpp fish.cpp menu.cpp player.cpp shark.cpp
OBJS := $(SRCS:.cpp=.o)

# Cible finale
TARGET := bloubloulespoissons.elf

# Règles
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
