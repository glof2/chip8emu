# Compiler to use
COMPILER = g++

# Debug?
DEBUG = 0

# Compiler flags:
# -Wall: Enable all warnings
# -Wextra: Enable extra warning flags
# -g: Include debugging info
# -std=c++20: use the c++20 standard

ifeq ($(DEBUG),1)
	CFLAGS = -Wall -Wextra -g -std=c++23
else
	CFLAGS = -O3 -std=c++23
endif


# Target executable name
TARGET = emulator

# Source file list
SOURCES = main.cpp ./source/Stack.cpp ./source/Chip8.cpp ./source/VarRegs.cpp ./source/Timer.cpp

# Additional libraries
LIBS = -lSDL2 -lSDL2_mixer



# Default build
all: $(TARGET)

# Create executable
$(TARGET): $(SOURCES)
	$(COMPILER) $(SOURCES) $(CFLAGS) $(LIBS) -o $(TARGET)

# Clear
clear:
	rm $(TARGET)

# Run
run:
	./$(TARGET)