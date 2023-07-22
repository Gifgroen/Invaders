@echo off
set CC=cl
set BASE_DIR=w:\Games\Invaders\

REM COMPILER flags
set COMMON_COMPILER_FLAGS=-EHsc -std:c++20 -O2 -Wall -ZI -WL

set COMMON_COMPILER_FLAGS=-DPLATFORM_WIN=1 %COMMON_COMPILER_FLAGS%


REM LINKING flags
set SDL2_LIB_DIR=%BASE_DIR%lib\SDL2-2.28.1\lib\x64
set COMMON_LINKER_FLAGS=%SDL2_LIB_DIR%\SDL2main.lib %SDL2_LIB_DIR%\SDL2.lib

REM INCLUDE setup for: SDL2
set SDL2_INCLUDE_DIR=%BASE_DIR%lib\SDL2-2.28.1\include\

REM BUILD setup
set BUILD_DIR=%BASE_DIR%\build\win
REM Create build folder and cd to it.
if not exist %BUILD_DIR% mkdir %BUILD_DIR%
pushd %BUILD_DIR%

REM TODO: see how we build a DDL from this
REM Create Game service
REM %CC% %COMMON_COMPILER_FLAGS% -shared -o ./libgame.dll? -fPIC $BASE_DIR/src/game.cc 

%CC% %COMMON_COMPILER_FLAGS% /I%SDL2_INCLUDE_DIR% %BASE_DIR%src\main.win.cc %COMMON_LINKER_FLAGS%

cp %SDL2_LIB_DIR%\SDL2.dll .

popd
