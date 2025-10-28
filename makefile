# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Iinclude -pthread

# Source files - FIXED: Use WebAPI.cpp instead of main.cpp
SOURCES = src/TrieNode.cpp src/Trie.cpp src/WebAPI.cpp

# Output executable name
TARGET = autocomplete_system

# Build the program
$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

# Clean up generated files
clean:
	rm -f $(TARGET)

# Specify that 'clean' is not a file
.PHONY: clean
