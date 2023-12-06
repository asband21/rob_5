# Makefile for Integrated C++ Program

# Compiler settings
CXX = g++
CXXFLAGS = -Wall -std=c++11

# Target binary
TARGET = tutel

# Source files
SOURCES = tutel.cpp 

# Object files
OBJECTS = $(SOURCES:.cpp=.o)

# Default rule
all: $(TARGET)

# Link the target binary
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS) -lpthread

# Compile the source files into object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(TARGET) $(OBJECTS)

.PHONY: all clean
