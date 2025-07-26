# Makefile for TCL Script Debugger
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
TARGET = tcl_debugger.exe
SOURCE = tcl_debugger.cpp

# Default target
all: $(TARGET)

# Build the debugger
$(TARGET): $(SOURCE)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCE)
	@echo "Build complete: $(TARGET)"

# Clean build artifacts
clean:
	del /f $(TARGET) 2>nul || true
	@echo "Clean complete"

# Test with sample script
test: $(TARGET)
	@echo "Testing formatted debugger..."
	.\$(TARGET)

# Run with specific script
run: $(TARGET)
	.\$(TARGET) ..\test_memory_debug.tcl

.PHONY: all clean test run
