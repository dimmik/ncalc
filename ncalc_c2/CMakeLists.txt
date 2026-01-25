cmake_minimum_required(VERSION 3.10)

project(NCalc)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(ncalc main.cpp)

# Link against Win32 libraries
target_link_libraries(ncalc PRIVATE user32 gdi32 comctl32 shell32)

# Set the subsystem to Windows
set_target_properties(ncalc PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
