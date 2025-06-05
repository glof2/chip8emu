# Compiler to use
COMPILER = g++

# Compiler flags:
# -Wall: Enable all warnings
# -Wextra: Enable extra warning flags
# -g: Include debugging info
# -std=c++20: use the c++20 standard
FLAGS = -Wall -Wextra -g -std=c++23

# Target executable name
TARGET = output_build

# Source file list
SOURCES = main.cpp Stack.cpp Chip8.cpp VarRegs.cpp Timer.cpp

# Additional libraries
LIBS = -lSDL2

# Default build
all: $(TARGET)

# Create executable
$(TARGET): $(SOURCES)
	$(COMPILER) $(SOURCES) $(FLAGS) $(LIBS) -o $(TARGET)
