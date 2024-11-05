# DevkitPro and DevkitARM path
DEVKITPRO ?= /opt/devkitpro
DEVKITARM ?= $(DEVKITPRO)/devkitARM

# Project name
TARGET := bloubloulespoissons
BUILD := build
SOURCES := .
INCLUDES := .

# Libraries
LIBS := -lSDL2 -lSDL2_image -lSDL2_ttf

# Compiler and linker settings
ARCH := -march=armv6k -mtune=mpcore -mfloat-abi=hard -mtp=soft
CFLAGS := -g -O2 -Wall -mword-relocations $(ARCH) -I$(DEVKITPRO)/libctru/include -I$(DEVKITPRO)/portlibs/3ds/include -I$(DEVKITPRO)/portlibs/3ds/include/SDL
LDFLAGS := -L$(DEVKITPRO)/libctru/lib -L$(DEVKITPRO)/portlibs/3ds/lib -lSDL -lSDL_image -lSDL_ttf

# Rules
all: $(BUILD)/$(TARGET).3dsx

$(BUILD)/$(TARGET).3dsx: $(BUILD)/$(TARGET).elf
	3dsxtool $(BUILD)/$(TARGET).elf $@

$(BUILD)/$(TARGET).elf: $(SOURCES)/*.cpp
	$(DEVKITARM)/bin/arm-none-eabi-g++ $(CFLAGS) -o $@ $^ $(LDFLAGS)
