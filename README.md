# TCL Script Debugger

A professional, memory-aware TCL script debugger with enhanced terminal output formatting.

## Features

- **Memory-level variable tracking** with simulated addresses
- **Real-time monitoring** with change detection
- **Scope management** for function calls
- **Type-specific analysis** (integers, strings, lists, dictionaries)
- **Breakpoint management** (line-based and variable-based)
- **Call stack visualization** with local variables
- **Professional terminal formatting** with colors and alignment
- **Cross-platform support** (Windows, Linux, macOS)

## Quick Start

### Windows
```cmd
make
tcl_debugger.exe
```

### Unix/Linux/macOS
```bash
# Automated setup
chmod +x setup_unix.sh
./setup_unix.sh

# Manual setup
cp Makefile.unix Makefile
make
./tcl_debugger
```

For detailed Unix installation instructions, see [UNIX_INSTALLATION.md](UNIX_INSTALLATION.md).

## Files

- `tcl_debugger.cpp` - Main source code
- `tcl_debugger.exe` - Windows executable
- `Makefile` - Windows build configuration
- `Makefile.unix` - Unix/Linux/macOS build configuration  
- `setup_unix.sh` - Automated Unix setup script
- `test_memory_debug.tcl` - Comprehensive test script
- `MEMORY_DEBUGGER_ARCHITECTURE.md` - Technical documentation
- `UNIX_INSTALLATION.md` - Detailed Unix setup guide

## Building

```bash
make
```

## Usage

```bash
# Interactive mode
./tcl_debugger.exe

# Load and debug a script
echo "load test_memory_debug.tcl" | ./tcl_debugger.exe

# Run comprehensive test
make test
```

## Commands

- `load <file>` - Load TCL script for debugging
- `run` - Start/resume script execution
- `step` - Step into next line
- `break <line>` - Set breakpoint at line number
- `vars` - List all variables with details
- `examine <var>` - Detailed variable analysis
- `memory <var>` - Show memory analysis
- `watch <var>` - Add variable to watch list
- `context [lines]` - Show source code context
- `stack` - Show call stack
- `help` - Show all commands
- `quit` - Exit debugger

## Example Output

```
============================================================
    TCL SCRIPT DEBUGGER v3.0 (Formatted Edition)
    Enhanced memory-level debugging with clean output
============================================================

[CREATE] counter         = '42' [INT] @0x207b1479 (11B, global, line 10)
[UPDATE] counter         = '43' [INT] @0x207b1479 (line 20) [was: '42']
[WATCH] Variable 'counter' changed: '42' -> '43'
```

## Architecture

Built with C++17, featuring:
- Modular class-based design
- ANSI color terminal support
- Robust input handling
- Memory simulation for debugging insights
- Type-aware variable analysis
