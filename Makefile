CXX := C:/raylib/w64devkit/bin/g++.exe

CXXFLAGS := -std=c++17 -Wall -Wextra -IC:/raylib/w64devkit/include
LDFLAGS := -LC:/raylib/w64devkit/lib
LDLIBS := -lraylib -lopengl32 -lgdi32 -lwinmm

TARGET := BossBattle.exe
SOURCE := main.cpp

$(TARGET): $(SOURCE)
	$(CXX) $(CXXFLAGS) $(SOURCE) -o $(TARGET) $(LDFLAGS) $(LDLIBS)

run: $(TARGET)
	./$(TARGET)