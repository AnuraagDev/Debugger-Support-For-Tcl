#!/bin/bash

# TCL Debugger Setup Script for Unix/Linux/macOS
# This script helps set up the debugger on Unix-like systems

set -e  # Exit on any error

echo "============================================================"
echo "    TCL Script Debugger - Unix Setup"
echo "============================================================"

# Check if we're on a Unix-like system
if [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "cygwin" ]] || [[ "$OSTYPE" == "win32" ]]; then
    echo "❌ This script is for Unix/Linux/macOS systems only."
    echo "   For Windows, use the existing Makefile and tcl_debugger.exe"
    exit 1
fi

echo "🔍 Checking prerequisites..."

# Check for g++
if ! command -v g++ &> /dev/null; then
    echo "❌ g++ not found. Please install:"
    echo "   Ubuntu/Debian: sudo apt install build-essential"
    echo "   CentOS/RHEL:   sudo yum groupinstall \"Development Tools\""
    echo "   macOS:         xcode-select --install"
    exit 1
fi

# Check for make
if ! command -v make &> /dev/null; then
    echo "❌ make not found. Please install build tools."
    exit 1
fi

# Check g++ version for C++17 support
GCC_VERSION=$(g++ -dumpversion | cut -d. -f1)
if [ "$GCC_VERSION" -lt 7 ]; then
    echo "⚠️  Warning: g++ version $GCC_VERSION may not support C++17"
    echo "   The debugger will try to build with C++14 instead"
    sed -i.bak 's/-std=c++17/-std=c++14/' Makefile.unix
fi

echo "✅ Prerequisites check passed"

# Use Unix Makefile
echo "📝 Setting up Unix Makefile..."
if [ -f "Makefile" ]; then
    mv Makefile Makefile.windows
    echo "   → Backed up Windows Makefile to Makefile.windows"
fi
cp Makefile.unix Makefile
echo "   → Activated Unix Makefile"

# Build the debugger
echo "🔨 Building debugger..."
make clean 2>/dev/null || true
make

# Test the build
echo "🧪 Testing debugger..."
if echo "help" | timeout 5s ./tcl_debugger > /dev/null 2>&1; then
    echo "✅ Build successful!"
else
    echo "❌ Build test failed"
    exit 1
fi

# Make executable (redundant but safe)
chmod +x tcl_debugger

echo "🎉 Setup complete!"
echo ""
echo "Usage:"
echo "  ./tcl_debugger                    # Interactive mode"
echo "  echo 'help' | ./tcl_debugger     # Show commands"
echo "  echo 'vars' | ./tcl_debugger     # Quick test"
echo "  make install                      # Install system-wide (optional)"
echo ""
echo "For detailed instructions, see: UNIX_INSTALLATION.md"
