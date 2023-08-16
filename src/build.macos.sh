#!/bin/sh
CC=g++

BASE_DIR="${HOME}/Projects/Games/Invaders/"

COMMON_COMPILER_FLAGS="-g3 -Wall -std=c++20"
BUILD_FLAGS="-DDEBUG=1 -DPLATFORM_MACOS=1"

SDL_FLAGS="`sdl2-config --cflags --libs`"

mkdir -p $BASE_DIR/build/macos
pushd $BASE_DIR/build/macos

# Create Game service
$CC $COMMON_COMPILER_FLAGS $BUILD_FLAGS -shared -o ./libgame.so -fPIC $BASE_DIR/src/game.cc 

# Create platform layer that uses platform agnostic Game
$CC $COMMON_COMPILER_FLAGS $BUILD_FLAGS -o invaders $SDL_FLAGS -fPIC $BASE_DIR/src/main.cc

popd
