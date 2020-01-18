# Makefile for building DuckChase for m68k-AmigaOS on Linux using 
# gcc 6.5 from this toolchain: https://github.com/bebbo/amiga-gcc
#
# Related forum thread: http://eab.abime.net/showthread.php?t=85474
# 
#
# Author: Uwe Rosner
# Date: 17.01.2019
#

# Setting up application name and directories
APPNAME=DuckChase
APPNAME_DEBUG = DuckChase_debug

SRC_ROOT=src

OBJ_DIR=obj_release
OBJ_DIR_DEBUG=obj_debug

# Change some values for debug build
ifeq ($(BUILD),debug)
OBJ_DIR=$(OBJ_DIR_DEBUG)
APPNAME=$(APPNAME_DEBUG)
endif

DEPS=$(SRC_ROOT)/amigautils/Picture.h \
		$(SRC_ROOT)/amigautils/StopWatch.h \
		$(SRC_ROOT)/animtools/animtools.h \
		$(SRC_ROOT)/animtools/animtools_proto.h \
		$(SRC_ROOT)/entities/Bullet.h \
		$(SRC_ROOT)/entities/Duck.h \
		$(SRC_ROOT)/entities/Hunter.h \
		$(SRC_ROOT)/gameengine/EntityBase.h \
		$(SRC_ROOT)/gameengine/EntityBob.h \
		$(SRC_ROOT)/gameengine/EntitySprite.h \
		$(SRC_ROOT)/gameengine/GameViewGfxLib.h \
		$(SRC_ROOT)/gameengine/GameViewIntui20.h \
		$(SRC_ROOT)/gameengine/GameViewLowLevel.h \
		$(SRC_ROOT)/gameengine/IGameView.h \
		$(SRC_ROOT)/lowlevelview/lowlevelview.h \
		$(SRC_ROOT)/lowlevelview/lowlevelviewport.h \
		$(SRC_ROOT)/Game.h \
		$(SRC_ROOT)/PointsDisplay.h

_OBJ=$(SRC_ROOT)/amigautils/Picture.o \
		$(SRC_ROOT)/amigautils/StopWatch.o \
		$(SRC_ROOT)/animtools/animtools.o \
		$(SRC_ROOT)/entities/Bullet.o \
		$(SRC_ROOT)/entities/Duck.o \
		$(SRC_ROOT)/entities/Hunter.o \
		$(SRC_ROOT)/gameengine/EntityBase.o \
		$(SRC_ROOT)/gameengine/EntityBob.o \
		$(SRC_ROOT)/gameengine/EntitySprite.o \
		$(SRC_ROOT)/gameengine/GameViewGfxLib.o \
		$(SRC_ROOT)/gameengine/GameViewIntui20.o \
		$(SRC_ROOT)/gameengine/GameViewLowLevel.o \
		$(SRC_ROOT)/lowlevelview/lowlevelview.o \
		$(SRC_ROOT)/lowlevelview/lowlevelviewport.o \
		$(SRC_ROOT)/Game.o \
		$(SRC_ROOT)/main.o \
		$(SRC_ROOT)/PointsDisplay.o
 
INCLUDE=-I$(SRC_ROOT) \
				-I$(SRC_ROOT)/amigautils \
				-I$(SRC_ROOT)/animtools \
				-I$(SRC_ROOT)/entities \
				-I$(SRC_ROOT)/gameengine \
				-I$(SRC_ROOT)/lowlevelview \
				-I$(SRC_ROOT)/

# Setting up compiler, flags and tools
CXX=/opt/amiga/bin/m68k-amigaos-c++
CXXFLAGS=-Wall -Wno-unused-function -fomit-frame-pointer -fno-rtti -fno-exceptions -noixemul -D LINUX
LD=/opt/amiga/bin/m68k-amigaos-ld
LDFLAGS=

ifeq ($(BUILD),debug)
# "Debug" build - no optimization, and debugging symbols
CXXFLAGS += -O0 -g -Wl,--amiga-debug-hunk -ldebug
#LDFLAGS += -Wl,--amiga-debug-hunk -ldebug
else
# "Release" build - optimization and no debug symbols
CXXFLAGS += -Os -s
endif

# Replace prefix SRC_ROOT with OBJ_DIR for object file target
OBJ=$(patsubst $(SRC_ROOT)/%,$(OBJ_DIR)/%,$(_OBJ))

# Building the .o files
# Note: the mkdir line creates the needed subdirectories in OB_DIR 
$(OBJ_DIR)/%.o: $(SRC_ROOT)/%.cpp $(DEPS)
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c -o $@ $< 


$(APPNAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -o $@ $^


debug:
	make "BUILD=debug"

release:
	make

# Cleaning build directory and executable
.PHONY: clean
clean:
	rm -rf $(OBJ_DIR) $(OBJ_DIR_DEBUG) $(APPNAME) $(APPNAME_DEBUG)
