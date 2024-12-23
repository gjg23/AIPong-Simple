# Compiler settings
CXX = g++

# Target executable name
TARGET = pong_game

# Compiler flags
CXX_FLAGS = -Wall -std=c++17
SDL_LIBS = -lSDL2 -lSDL2_ttf
LIBS = $(SDL_LIBS) -lm

# Debug flags
CXX_FLAGS_DEBUG = -g -DDEBUG

# Source files
CPP_SOURCES = main.cpp pong.cpp train-data.cpp network.cpp
HEADERS = pong.h train-data.h network.h config.h

# Object files
CPP_OBJECTS = $(CPP_SOURCES:.cpp=.o)
ALL_OBJECTS = $(CPP_OBJECTS)

# Default target
all: $(TARGET)

# Debug target
debug: CXX_FLAGS += $(CXX_FLAGS_DEBUG)
debug: $(TARGET)

# Link the target executable
$(TARGET): $(ALL_OBJECTS)
		$(CXX) $(ALL_OBJECTS) -o $(TARGET) $(LIBS)

# Compile C++ source files
%.o: %.cpp $(HEADERS)
		$(CXX) $(CXX_FLAGS) -c $< -o $@

# Clean target
clean:
		rm -f $(ALL_OBJECTS) $(TARGET)

# Run target
run: $(TARGET)
		./$(TARGET)

# Debug run target
debug-run: debug
		gdb ./$(TARGET)

# Phony targets
.PHONY: all clean run debug debug-run

# Dependencies
main.o: main.cpp $(HEADERS)
pong.o: pong.cpp pong.h config.h
train-data.o: train-data.cpp train-data.h
network.o: network.cpp network.h