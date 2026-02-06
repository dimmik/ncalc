@echo off
echo Compiling ncalc...
g++ -std=c++17 -Wall -Wextra -O2 -c main.cpp -o main.o
if errorlevel 1 (
    echo Compilation failed!
    pause
    exit /b 1
)

echo Linking...
g++ main.o -o ncalc.exe -mwindows -luser32 -lgdi32 -lcomctl32 -lshell32 -lcomdlg32
if errorlevel 1 (
    echo Linking failed!
    pause
    exit /b 1
)

echo Cleaning up...
del main.o

echo Build complete! ncalc.exe created successfully.
echo.
echo To run the calculator, execute: ncalc.exe
pause