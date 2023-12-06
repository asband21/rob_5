# Makefile for Integrated C++ Program

# Compiler settings
CXX = gcc
CXXFLAGS = -Wall 

# Target binary
TARGET = tutel

# Source files
SOURCES = tutel.c 

# Object files
OBJECTS = $(SOURCES:.cpp=.o)

# Default rule
all: $(TARGET)

# Link the target binary
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS) -lpthread

# Compile the source files into object files
%.o: %.c
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(TARGET) $(OBJECTS)

.PHONY: all clean
