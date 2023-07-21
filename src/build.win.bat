@echo off

set BASE_DIR=w:\Games\Invaders\

set COMMON_COMPILER_FLAGS="/std:c++20 /Wall"

set BUILD_DIR=%BASE_DIR%\build\win
set SDL2_LIB_DIR=%BASE_DIR%lib\SDL2-2.28.1\lib\x64
set SDL2_INCLUDE_DIR=%BASE_DIR%lib\SDL2-2.28.1\include\

if not exist %BUILD_DIR% mkdir %BUILD_DIR%
pushd %BUILD_DIR%

cl %COMMON_COMPILER_FLAGS% -EHsc /I%SDL2_INCLUDE_DIR% /ZI %BASE_DIR%src\main.win.cc %SDL2_LIB_DIR%\SDL2main.lib %SDL2_LIB_DIR%\SDL2.lib

cp %SDL2_LIB_DIR%\SDL2.dll .

popd
