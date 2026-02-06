# Makefile for ncalc Win32 Calculator
# Uses g++ compiler on Windows

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
LDFLAGS = -mwindows -luser32 -lgdi32 -lcomctl32 -lshell32 -lcomdlg32

TARGET = ncalc.exe
SOURCES = main.cpp
OBJECTS = $(SOURCES:.cpp=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	del /Q *.o $(TARGET) 2>nul || rm -f *.o $(TARGET)

install: $(TARGET)
	copy $(TARGET) .\

run: $(TARGET)
	./$(TARGET)

debug: CXXFLAGS += -g -DDEBUG
debug: $(TARGET)