# Define variables
CXX = g++
CXXFLAGS = -fopenmp
TARGET = main
SRC = main.cpp

# Default target
all: $(TARGET)

# Build target
$(TARGET): $(SRC)
	$(CXX) $(SRC) $(CXXFLAGS) -o $(TARGET)

# Clean target
clean:
	rm -f $(TARGET)
