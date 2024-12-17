# Chemins des outils devkitPro et devkitPPC
DEVKITPRO ?= /opt/devkitpro
DEVKITPPC ?= $(DEVKITPRO)/devkitPPC

# Chemins des bibliothèques
INCLUDES := -I$(DEVKITPRO)/portlibs/wiiu/include -I$(DEVKITPRO)/portlibs/wiiu/include/SDL2
LIBS := -L$(DEVKITPRO)/portlibs/wiiu/lib -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_net -lSDL2_mixer -lm -lwiiload -lGX2 -lprocui

# Compilateurs
CC := $(DEVKITPPC)/bin/powerpc-eabi-gcc
CXX := $(DEVKITPPC)/bin/powerpc-eabi-g++
LD := $(DEVKITPPC)/bin/powerpc-eabi-ld

# Options de compilation
CFLAGS := -O2 -g -std=gnu11 -m32
CXXFLAGS := $(CFLAGS) -std=c++23
LDFLAGS := -specs=rp2040.specs

# Fichiers sources et objets
SRCS := main.cpp camera.cpp decors.cpp display.cpp env.cpp fish.cpp menu.cpp player.cpp shark.cpp
OBJS := $(SRCS:.cpp=.o)

# Cible finale
TARGET := bloubloulespoissons.elf

# Règles
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
