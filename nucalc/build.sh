#!/usr/bin/env bash
# Builds nucalc.exe with MSYS2 UCRT64 g++ (from WSL or MSYS2 shell).
set -e
cd "$(dirname "$0")"

TOOLDIR=/mnt/c/msys64/ucrt64/bin
[ -d "$TOOLDIR" ] || TOOLDIR=/ucrt64/bin   # when run inside MSYS2 itself
GCC="$TOOLDIR/gcc.exe"
GXX="$TOOLDIR/g++.exe"

COMMON="-Os -flto -DNDEBUG -ffunction-sections -fdata-sections -Wall"

"$GCC" $COMMON -c tinyexpr.c -o tinyexpr.o
"$GXX" $COMMON -std=gnu++17 -municode -DUNICODE -D_UNICODE \
       -fno-exceptions -fno-rtti -c main.cpp -o main.o
"$GXX" -municode -mwindows -Os -flto -fno-exceptions -fno-rtti \
       -static -s -Wl,--gc-sections \
       main.o tinyexpr.o -o nucalc.exe \
       -lgdi32 -lshell32 -ladvapi32 -ldwmapi

ls -la nucalc.exe
