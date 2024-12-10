# Define variables
CXX = g++
CXXFLAGS = -fopenmp
INCLUDES = -I"C:/Program Files (x86)/Intel/oneAPI/ipp/latest/include"
LIBS = -L"C:/Program Files (x86)/Intel/oneAPI/ipp/latest/lib" -lippcore
TARGET = main1
SRC = main1.cpp

# Default target
all: $(TARGET)

# Build target
$(TARGET): $(SRC)
	$(CXX) $(SRC) $(CXXFLAGS) $(INCLUDES) $(LIBS) -o $(TARGET)

# Clean target
clean:
	rm -f $(TARGET)
