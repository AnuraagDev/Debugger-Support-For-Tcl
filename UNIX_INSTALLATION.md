# TCL Script Debugger - Unix/Linux Installation Guide

## Prerequisites

### Required Tools
```bash
# Ubuntu/Debian
sudo apt update
sudo apt install build-essential g++ make

# CentOS/RHEL/Fedora
sudo yum groupinstall "Development Tools"
# or for newer versions:
sudo dnf groupinstall "Development Tools"

# macOS
xcode-select --install
# or install via Homebrew:
brew install gcc make
```

### Verify Installation
```bash
g++ --version    # Should show version 7.0+ for C++17 support
make --version   # GNU Make or compatible
```

## Installation Steps

### 1. Download/Clone the Project
```bash
# If you have the files, copy them to your Unix system
scp -r /path/to/Debuggertcl user@unix-server:~/
# or
rsync -av /path/to/Debuggertcl/ user@unix-server:~/tcl-debugger/

# Or download individual files:
mkdir tcl-debugger
cd tcl-debugger
# Copy the required files here
```

### 2. Modify Makefile for Unix
The current Makefile uses Windows-specific commands. Create a Unix-compatible version:

```bash
# Backup original Makefile
cp Makefile Makefile.windows

# Create Unix Makefile
cat > Makefile << 'EOF'
# Makefile for TCL Script Debugger (Unix/Linux)
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
TARGET = tcl_debugger
SOURCE = tcl_debugger.cpp

# Default target
all: $(TARGET)

# Build the debugger
$(TARGET): $(SOURCE)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCE)
	@echo "Build complete: $(TARGET)"

# Clean build artifacts
clean:
	rm -f $(TARGET)
	@echo "Clean complete"

# Test with sample script
test: $(TARGET)
	@echo "Testing debugger..."
	./$(TARGET)

# Run with specific script
run: $(TARGET)
	./$(TARGET) test_memory_debug.tcl

# Install to system (optional)
install: $(TARGET)
	sudo cp $(TARGET) /usr/local/bin/
	@echo "Installed to /usr/local/bin/"

# Uninstall from system
uninstall:
	sudo rm -f /usr/local/bin/$(TARGET)
	@echo "Uninstalled from /usr/local/bin/"

.PHONY: all clean test run install uninstall
EOF
```

### 3. Build the Debugger
```bash
# Compile
make

# Make executable (if needed)
chmod +x tcl_debugger

# Test that it works
echo "help" | ./tcl_debugger
```

## Platform-Specific Modifications

### File Handling Changes
The code automatically handles Unix paths, but if you encounter issues, check:

1. **Path Separators**: Unix uses `/` instead of `\`
2. **Line Endings**: Unix uses `\n` instead of `\r\n`
3. **File Permissions**: Make sure scripts are readable

### Terminal Colors
The ANSI color codes work on most Unix terminals, but you can disable them if needed:

```bash
# Run without colors (if terminal doesn't support ANSI)
NO_COLOR=1 ./tcl_debugger

# Or modify the source to detect terminal capabilities
```

## Usage Examples

### Basic Usage
```bash
# Interactive mode
./tcl_debugger

# Debug a specific script
echo "load myscript.tcl" | ./tcl_debugger

# Batch debugging commands
cat > debug_session.txt << 'EOF'
load test_memory_debug.tcl
break 10
run
vars
examine counter
quit
EOF

./tcl_debugger < debug_session.txt
```

### Advanced Usage
```bash
# Debug with output logging
./tcl_debugger | tee debug_output.log

# Run in background and pipe commands
mkfifo debug_pipe
./tcl_debugger < debug_pipe &
echo "load myscript.tcl" > debug_pipe
echo "run" > debug_pipe
```

## Troubleshooting

### Common Issues

1. **Compilation Errors**
```bash
# If C++17 not supported, try C++14
sed -i 's/-std=c++17/-std=c++14/' Makefile
make clean && make
```

2. **Permission Denied**
```bash
chmod +x tcl_debugger
```

3. **Missing Libraries**
```bash
# The debugger only uses standard C++ libraries, no external dependencies needed
ldd tcl_debugger  # Check dynamic library dependencies (should be minimal)
```

4. **Terminal Color Issues**
```bash
# Check terminal capabilities
echo $TERM
tput colors

# Test colors manually
echo -e "\033[32mGreen Text\033[0m"
```

### Performance Considerations

1. **Memory Usage**: The debugger simulates memory addresses and may use more RAM for large scripts
2. **Terminal Speed**: ANSI colors may slow down output on some terminals
3. **File I/O**: Large TCL scripts may take time to load and parse

## Integration with Development Environment

### Vim Integration
```vim
" Add to ~/.vimrc
command! TclDebug !echo "load %" | ./tcl_debugger
nnoremap <F5> :TclDebug<CR>
```

### Emacs Integration
```elisp
;; Add to ~/.emacs
(defun tcl-debug-current-file ()
  "Debug current TCL file"
  (interactive)
  (shell-command (format "echo 'load %s' | ./tcl_debugger" (buffer-file-name))))

(global-set-key (kbd "C-c C-d") 'tcl-debug-current-file)
```

### Shell Aliases
```bash
# Add to ~/.bashrc or ~/.zshrc
alias tcldbg='./tcl_debugger'
alias tcltest='echo "load test_memory_debug.tcl" | ./tcl_debugger'

# Function for quick debugging
debug_tcl() {
    if [ -f "$1" ]; then
        echo "load $1" | ./tcl_debugger
    else
        echo "File not found: $1"
    fi
}
```

## Distribution

### Creating a Portable Package
```bash
# Create distribution package
mkdir tcl-debugger-unix
cp tcl_debugger tcl_debugger.cpp Makefile README.md test_memory_debug.tcl tcl-debugger-unix/
tar -czf tcl-debugger-unix.tar.gz tcl-debugger-unix/

# Users can then:
tar -xzf tcl-debugger-unix.tar.gz
cd tcl-debugger-unix
make
./tcl_debugger
```

### System-wide Installation
```bash
# Install for all users
sudo make install

# Now available system-wide
tcl_debugger

# Uninstall
sudo make uninstall
```

## Security Considerations

1. **File Permissions**: Ensure debugger doesn't have unnecessary privileges
2. **Script Execution**: The debugger simulates execution, doesn't actually run TCL code
3. **Network Access**: No network functionality, safe for air-gapped systems

This guide should help anyone get the TCL debugger running on Unix/Linux systems! 🐧
