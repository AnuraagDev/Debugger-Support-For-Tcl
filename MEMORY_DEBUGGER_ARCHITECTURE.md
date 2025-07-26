# TCL Memory-Level Debugger Architecture

## 🧠 Core Concept: Address-Level Debugging

This debugger works at the **memory level** by directly interfacing with TCL's internal structures and execution engine, rather than just parsing script text.

## 🏗️ Architecture Overview

### 1. Memory-Level Variable Tracking
```
TCL Variable in Memory:
┌─────────────────────────────────────┐
│ Tcl_Obj Structure                   │
├─────────────────────────────────────┤
│ refCount: 1                         │ ← Reference counting
│ bytes: "Hello World"                │ ← String representation  
│ length: 11                          │ ← Byte length
│ typePtr: &stringType                │ ← Type information
│ internalRep: {...}                  │ ← Type-specific data
└─────────────────────────────────────┘
Memory Address: 0x20b67917
```

**Key Features:**
- **Direct Memory Access**: Read variable values from TCL's internal hash tables
- **Type Analysis**: Detect actual TCL types (string, list, dict, int, double)
- **Reference Counting**: Track object lifecycle and memory usage
- **Memory Dumps**: Show raw bytes and hex representation

### 2. Execution Hooks and Breakpoints
```
TCL Command Execution Flow:
┌─────────────────┐    ┌──────────────────┐    ┌─────────────────┐
│ Command Parse   │───▶│ Our Hook         │───▶│ Original Command│
│ "set var value" │    │ (Intercept)      │    │ Execution       │
└─────────────────┘    └──────────────────┘    └─────────────────┘
                              │
                              ▼
                       ┌──────────────────┐
                       │ Variable Tracker │
                       │ Memory Analysis  │
                       │ Breakpoint Check │
                       └──────────────────┘
```

**Hooks Installed:**
- **Command Interception**: Hook into `set`, `puts`, `expr`, etc.
- **Variable Traces**: Monitor all variable modifications  
- **Execution Traces**: Track every command before execution
- **Memory Breakpoints**: Set breakpoints at specific memory addresses

### 3. Real-Time Memory Monitoring

#### Variable Memory Analysis
```cpp
struct VariableMemoryInfo {
    std::string name;              // Variable name
    Tcl_Obj* tclObject;           // Direct TCL object pointer
    void* memoryAddress;          // Memory location
    size_t memorySize;            // Size in bytes
    std::string type;             // TCL type
    int refCount;                 // Reference count
    std::vector<uint8_t> rawBytes; // Raw memory dump
};
```

#### Dictionary Memory Layout
```
Dictionary: {key1 value1 key2 value2}
┌─────────────────────────────────────┐
│ Tcl_Obj (dict type)                │
├─────────────────────────────────────┤
│ Hash Table Pointer ──────┐         │
└─────────────────────────┼─────────┘
                          ▼
                ┌─────────────────────────┐
                │ Hash Table              │
                ├─────────────────────────┤
                │ ["key1"] → Tcl_Obj*     │ ──▶ "value1" @0x40d82005
                │ ["key2"] → Tcl_Obj*     │ ──▶ "value2" @0x404ebb84
                └─────────────────────────┘
```

## 🛠️ Implementation Details

### TCL C API Integration

#### Real Version (with TCL)
```cpp
// Direct TCL interpreter access
Tcl_Interp* interp = Tcl_CreateInterp();

// Hook command execution
Tcl_Command setCmd = Tcl_FindCommand(interp, "set", nullptr, TCL_GLOBAL_ONLY);
Tcl_SetCommandInfo(interp, "set", hookedSetCommand);

// Read variable directly from memory
Tcl_Obj* varObj = Tcl_GetVar2Ex(interp, varName.c_str(), nullptr, TCL_GLOBAL_ONLY);
void* memAddr = varObj;  // Direct memory address
```

#### Simulation Version (for testing)
```cpp
// Simulated TCL structures
struct SimTcl_Obj {
    char* bytes;
    int length;
    void* typePtr;
    int refCount;
};

// Simulated memory addresses
void* memoryAddress = reinterpret_cast<void*>(0x20000000 + randomOffset);
```

### Memory Breakpoints

Unlike traditional line-based breakpoints, this debugger can set breakpoints at:
- **Memory Addresses**: Break when specific memory location is accessed
- **Variable Changes**: Break when a variable's memory content changes
- **Type Conversions**: Break when TCL converts between types
- **Reference Count Changes**: Break on object creation/destruction

```cpp
struct MemoryBreakpoint {
    int line;
    void* memoryAddress;      // Actual memory location
    std::string condition;    // Memory-based condition
    bool enabled;
    int hitCount;
};
```

## 🎯 Debugging Capabilities

### 1. Variable Memory Analysis
```bash
(memory-debug) track mydict
🔍 Tracking variable: mydict
  ✓ Variable tracked: mydict = '{key1 value1 key2 value2}' [dict] @0x203afe39 (refs: 1)

(memory-debug) mem mydict
🧠 === MEMORY ANALYSIS: mydict ===
📍 Memory Address: 0x203afe39
📏 Memory Size: 58 bytes
🏷️ Type: dict
📊 Ref Count: 1
💾 Value: '{key1 value1 key2 value2}'
🔧 Hex Dump:
    7b 6b 65 79 31 20 76 61 6c 75 65 31 20 6b 65 79
    32 20 76 61 6c 75 65 32 7d 8f 8d b9 2b 74 95 b0
📚 DICTIONARY MEMORY ANALYSIS:
  📏 Dictionary Size: 2 key-value pairs
  ['key1'] = 'value1' @0x404ebb84
  ['key2'] = 'value2' @0x40d82005
```

### 2. Real-Time Variable Monitoring
```bash
(memory-debug) set mynumber 42
📝 Variable SET: mynumber = 42
📝 VARIABLE CHANGED: mynumber = 42

(memory-debug) set mynumber 11  
🔄 Variable CHANGED: mynumber '42' → '11' @0x202ebe78
```

### 3. Type-Specific Analysis

#### Lists
```bash
📋 LIST MEMORY ANALYSIS:
  📏 List Length: 3 elements
  [0] = 'item1' @0x30721a8b
  [1] = 'item2' @0x30f42c19
  [2] = 'item3' @0x30891d57
```

#### Dictionaries  
```bash
📚 DICTIONARY MEMORY ANALYSIS:
  📏 Dictionary Size: 2 key-value pairs
  ['key1'] = 'value1' @0x404ebb84
  ['key2'] = 'value2' @0x40d82005
```

## 🔧 Building and Usage

### Requirements
- **TCL Development Headers**: `tcl.h` and related headers
- **TCL Library**: `libtcl8.6` or `tcl86.dll`
- **C++17 Compiler**: g++, clang++, or MSVC

### Build Commands
```bash
# Real version (requires TCL installation)
make -f Makefile_memory

# Simulation version (no TCL required)
make -f Makefile_memory simulation

# Test simulation
./tcl_memory_debugger_sim.exe
```

### Usage Examples
```bash
# Start memory debugging
./tcl_memory_debugger.exe script.tcl

# Interactive commands
(memory-debug) track myvar          # Track variable in memory
(memory-debug) mem myvar            # Show memory analysis
(memory-debug) break 15             # Set breakpoint at line 15
(memory-debug) set var "new value"  # Execute TCL command
(memory-debug) vars                 # Show all tracked variables
```

## 🚀 Advantages Over Traditional Debugging

### Traditional Debugger
- **Text-based**: Parses script text line by line
- **Simulated execution**: Guesses what variables contain
- **Limited type support**: Basic string/number detection
- **No memory insight**: Can't see actual memory usage

### Memory-Level Debugger
- **Memory-based**: Direct access to TCL's internal structures
- **Real execution**: Uses actual TCL interpreter
- **Full type support**: Native TCL types (dict, list, array, etc.)
- **Memory insight**: See actual memory addresses, sizes, reference counts
- **Hook-based**: Intercepts actual command execution
- **Real-time monitoring**: Track changes as they happen in memory

## 🔮 Future Enhancements

1. **Memory Leak Detection**: Track reference count changes
2. **Performance Profiling**: Monitor memory allocation patterns
3. **Call Stack Memory Analysis**: Show memory usage per function
4. **Garbage Collection Monitoring**: Track TCL's object cleanup
5. **Memory Diff**: Compare memory states between executions
6. **Interactive Memory Editor**: Modify variables directly in memory

## 💡 Use Cases

- **Performance Optimization**: Find memory-intensive operations
- **Memory Leak Debugging**: Track object lifecycle
- **Type System Understanding**: See how TCL handles type conversions
- **Complex Data Structure Analysis**: Debug nested lists/dicts
- **Real-time Variable Monitoring**: Watch variables change during execution
- **Educational Tool**: Understand TCL's internal memory management

This memory-level approach provides unprecedented insight into TCL script execution by working directly with the interpreter's memory structures rather than just parsing text.
