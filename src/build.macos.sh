#!/bin/sh
CC=g++

BASE_DIR="${HOME}/Projects/Games/Invaders"

# We add . relative to where to where the executable is so we can distribute it anywhere.
COMMON_COMPILER_FLAGS="-g3 -Wall -std=c++20 -Wl,-rpath,@executable_path/."

BUILD_FLAGS="-DDEBUG=1 -DPLATFORM_MACOS=1"

SDL_FLAGS="`sdl2-config --cflags --libs`"

mkdir -p $BASE_DIR/build/macos
pushd $BASE_DIR/build/macos

# Create Game service
$CC $COMMON_COMPILER_FLAGS $BUILD_FLAGS -shared -o ./libgame.so -fPIC $BASE_DIR/src/game.cc 

# Create platform layer that uses platform agnostic Game
$CC $COMMON_COMPILER_FLAGS $BUILD_FLAGS -o invaders $SDL_FLAGS -fPIC $BASE_DIR/src/main.cc

popd
