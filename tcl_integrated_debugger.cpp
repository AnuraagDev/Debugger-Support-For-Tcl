/*
 * TCL Integrated Debugger - Enhanced C++ Implementation
 * 
 * This debugger combines:
 * 1. Interactive debugging interface from tcl_debugger.cpp
 * 2. Memory-level variable tracking concepts
 * 3. Real TCL script execution with line-by-line control
 * 4. Enhanced variable analysis and type detection
 * 5. Breakpoint management with memory awareness
 * 
 * Architecture:
 * - TclIntegratedDebugger: Main orchestrator
 * - EnhancedBreakpointManager: Memory-aware breakpoints
 * - MemoryAwareVariableTracker: Advanced variable tracking
 * - ScriptExecutionController: Real script execution control
 * - InteractiveDebugConsole: Enhanced user interface
 */

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <fstream>
#include <sstream>
#include <memory>
#include <functional>
#include <regex>
#include <iomanip>
#include <cstring>
#include <random>
#include <chrono>

// Forward declarations
class TclIntegratedDebugger;

// Enhanced breakpoint structure with memory awareness
struct EnhancedBreakpoint {
    int line;
    std::string filename;
    std::string condition;
    bool enabled;
    int hitCount;
    
    // Memory-level features
    std::string watchVariable;      // Variable to watch for changes
    std::string memoryCondition;    // Memory-based condition
    void* simulatedAddress;         // Simulated memory address
    
    EnhancedBreakpoint() : line(0), enabled(true), hitCount(0), simulatedAddress(nullptr) {}
    EnhancedBreakpoint(int l, const std::string& file, const std::string& cond = "") 
        : line(l), filename(file), condition(cond), enabled(true), hitCount(0) {
        // Simulate memory address for demonstration
        static std::random_device rd;
        static std::mt19937 gen(rd());
        simulatedAddress = reinterpret_cast<void*>(0x10000000 + gen() % 0x1000000);
    }
};

// Enhanced variable information with memory-level details
struct EnhancedVariableInfo {
    std::string name;
    std::string value;
    std::string previousValue;
    std::string type;
    std::string scope;
    int lastModifiedLine;
    int accessCount;
    
    // Memory-level information
    void* simulatedAddress;
    size_t estimatedSize;
    int refCount;
    
    // Type-specific analysis
    bool isArray;
    bool isList;
    bool isDictionary;
    bool isNumeric;
    bool isEmpty;
    
    // Enhanced data structures
    std::vector<std::string> listElements;
    std::map<std::string, std::string> arrayElements;
    std::map<std::string, std::string> dictElements;
    std::vector<std::string> valueHistory;
    
    // Memory simulation
    std::vector<uint8_t> simulatedMemory;
    std::string hexDump;
    
    EnhancedVariableInfo() : name(""), value(""), previousValue(""), type(""), 
                           scope("global"), lastModifiedLine(0), accessCount(0),
                           simulatedAddress(nullptr), estimatedSize(0), refCount(1),
                           isArray(false), isList(false), isDictionary(false), 
                           isNumeric(false), isEmpty(true) {}
    
    EnhancedVariableInfo(const std::string& n, const std::string& v, const std::string& s = "global") 
        : name(n), value(v), previousValue(""), type(""), scope(s), 
          lastModifiedLine(0), accessCount(0), refCount(1),
          isArray(false), isList(false), isDictionary(false), 
          isNumeric(false), isEmpty(v.empty()) {
        
        // Simulate memory address
        static std::random_device rd;
        static std::mt19937 gen(rd());
        simulatedAddress = reinterpret_cast<void*>(0x20000000 + gen() % 0x1000000);
        
        analyzeTypeAndStructure();
        generateMemorySimulation();
    }
    
    void updateValue(const std::string& newValue, int line = 0) {
        if (!valueHistory.empty() || value != newValue) {
            valueHistory.push_back(value);
            if (valueHistory.size() > 10) {
                valueHistory.erase(valueHistory.begin());
            }
        }
        
        previousValue = value;
        value = newValue;
        lastModifiedLine = line;
        accessCount++;
        isEmpty = newValue.empty();
        
        analyzeTypeAndStructure();
        generateMemorySimulation();
    }
    
    void analyzeTypeAndStructure() {
        // Reset type flags
        isArray = false;
        isList = false;
        isDictionary = false;
        isNumeric = false;
        
        if (isEmpty) {
            type = "empty";
            return;
        }
        
        // Check if numeric
        if (isNumericValue(value)) {
            isNumeric = true;
            type = (value.find('.') != std::string::npos) ? "float" : "integer";
            return;
        }
        
        // Check if it's a dictionary
        if (isDictionaryValue(value)) {
            isDictionary = true;
            type = "dictionary";
            parseDictionary();
            return;
        }
        
        // Check if it's a list
        if (isListValue(value)) {
            isList = true;
            type = "list";
            parseList();
            return;
        }
        
        // Default to string
        type = "string";
    }
    
    std::string getTypeIcon() const {
        if (type == "integer") return "🔢";
        if (type == "float") return "🔣";
        if (type == "string") return "📝";
        if (type == "list") return "📋";
        if (type == "dictionary") return "📚";
        if (type == "array") return "🗂️";
        if (type == "empty") return "🗳️";
        return "❓";
    }
    
    std::string getDetailedTypeInfo() const {
        std::string info = type;
        if (isList && !listElements.empty()) {
            info += " (" + std::to_string(listElements.size()) + " elements)";
        } else if (isDictionary && !dictElements.empty()) {
            info += " (" + std::to_string(dictElements.size()) + " pairs)";
        } else if (isArray && !arrayElements.empty()) {
            info += " (" + std::to_string(arrayElements.size()) + " entries)";
        }
        return info;
    }
    
    std::string getMemoryInfo() const {
        std::ostringstream ss;
        ss << "@" << simulatedAddress << " (" << estimatedSize << " bytes, refs: " << refCount << ")";
        return ss.str();
    }
    
private:
    bool isNumericValue(const std::string& val) const {
        if (val.empty()) return false;
        
        try {
            std::stod(val);
            return true;
        } catch (...) {
            return false;
        }
    }
    
    bool isListValue(const std::string& val) const {
        if (val.empty()) return false;
        
        // Simple heuristic: check for space-separated elements or braces
        if ((val.front() == '{' && val.back() == '}') || val.find(' ') != std::string::npos) {
            return !isDictionaryValue(val);  // Not a dictionary
        }
        return false;
    }
    
    bool isDictionaryValue(const std::string& val) const {
        if (val.empty()) return false;
        
        std::string cleanVal = val;
        if (cleanVal.front() == '{' && cleanVal.back() == '}') {
            cleanVal = cleanVal.substr(1, cleanVal.length() - 2);
        }
        
        std::vector<std::string> elements;
        std::istringstream iss(cleanVal);
        std::string element;
        while (iss >> element) {
            elements.push_back(element);
        }
        
        // Must have even number of elements for key-value pairs
        return elements.size() % 2 == 0 && elements.size() >= 2 && elements.size() <= 20;
    }
    
    void parseList() {
        listElements.clear();
        std::string cleanValue = value;
        
        if (cleanValue.front() == '{' && cleanValue.back() == '}') {
            cleanValue = cleanValue.substr(1, cleanValue.length() - 2);
        }
        
        std::istringstream iss(cleanValue);
        std::string element;
        while (iss >> element) {
            listElements.push_back(element);
        }
    }
    
    void parseDictionary() {
        dictElements.clear();
        std::string cleanValue = value;
        
        if (cleanValue.front() == '{' && cleanValue.back() == '}') {
            cleanValue = cleanValue.substr(1, cleanValue.length() - 2);
        }
        
        std::vector<std::string> elements;
        std::istringstream iss(cleanValue);
        std::string element;
        while (iss >> element) {
            elements.push_back(element);
        }
        
        // Parse key-value pairs
        for (size_t i = 0; i + 1 < elements.size(); i += 2) {
            dictElements[elements[i]] = elements[i + 1];
        }
    }
    
    void generateMemorySimulation() {
        estimatedSize = sizeof(void*) + value.length() + 1;  // Simulate object overhead
        
        // Generate simulated memory content
        simulatedMemory.clear();
        for (char c : value) {
            simulatedMemory.push_back(static_cast<uint8_t>(c));
        }
        
        // Add some simulated padding
        static std::random_device rd;
        static std::mt19937 gen(rd());
        for (int i = 0; i < 8; i++) {
            simulatedMemory.push_back(gen() % 256);
        }
        
        // Generate hex dump
        std::ostringstream hexStream;
        hexStream << "    ";
        for (size_t i = 0; i < std::min(simulatedMemory.size(), size_t(32)); i++) {
            if (i > 0 && i % 16 == 0) {
                hexStream << "\n    ";
            }
            hexStream << std::hex << std::setw(2) << std::setfill('0') 
                      << static_cast<int>(simulatedMemory[i]) << " ";
        }
        if (simulatedMemory.size() > 32) {
            hexStream << "\n    ... (+" << (simulatedMemory.size() - 32) << " more bytes)";
        }
        hexDump = hexStream.str();
    }
};

// Stack frame for procedure calls
struct EnhancedStackFrame {
    std::string functionName;
    int line;
    std::string filename;
    std::map<std::string, EnhancedVariableInfo> localVariables;
    void* simulatedFrameAddress;
    
    EnhancedStackFrame(const std::string& func, int l, const std::string& file = "") 
        : functionName(func), line(l), filename(file) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        simulatedFrameAddress = reinterpret_cast<void*>(0x7fff0000 + gen() % 0x10000);
    }
};

// Enhanced Breakpoint Manager
class EnhancedBreakpointManager {
private:
    std::map<int, EnhancedBreakpoint> breakpoints;
    std::vector<std::string> watchedVariables;
    
public:
    void addBreakpoint(int line, const std::string& filename = "", const std::string& condition = "") {
        breakpoints[line] = EnhancedBreakpoint(line, filename, condition);
        std::cout << "🔴 Enhanced breakpoint set at line " << line;
        if (!filename.empty()) std::cout << " in " << filename;
        if (!condition.empty()) std::cout << " (condition: " << condition << ")";
        std::cout << " @" << breakpoints[line].simulatedAddress << std::endl;
    }
    
    void addVariableWatchBreakpoint(int line, const std::string& varName, const std::string& condition = "") {
        EnhancedBreakpoint bp(line, "", condition);
        bp.watchVariable = varName;
        bp.memoryCondition = condition;
        breakpoints[line] = bp;
        
        std::cout << "👁️ Variable watch breakpoint set at line " << line 
                  << " for variable '" << varName << "'";
        if (!condition.empty()) std::cout << " (condition: " << condition << ")";
        std::cout << " @" << bp.simulatedAddress << std::endl;
    }
    
    void removeBreakpoint(int line) {
        auto it = breakpoints.find(line);
        if (it != breakpoints.end()) {
            breakpoints.erase(it);
            std::cout << "🔴 Breakpoint removed from line " << line << std::endl;
        } else {
            std::cout << "❌ No breakpoint found at line " << line << std::endl;
        }
    }
    
    bool hasBreakpoint(int line) {
        auto it = breakpoints.find(line);
        return it != breakpoints.end() && it->second.enabled;
    }
    
    bool checkVariableWatchBreakpoint(const std::string& varName, const std::string& oldValue, const std::string& newValue) {
        for (auto& [line, bp] : breakpoints) {
            if (bp.enabled && bp.watchVariable == varName) {
                bp.hitCount++;
                
                // Check memory condition if specified
                if (!bp.memoryCondition.empty()) {
                    // Simple condition checking (can be enhanced)
                    if (bp.memoryCondition == "changed" && oldValue != newValue) {
                        return true;
                    }
                    if (bp.memoryCondition.find("=") != std::string::npos) {
                        std::string expectedValue = bp.memoryCondition.substr(bp.memoryCondition.find("=") + 1);
                        if (newValue == expectedValue) {
                            return true;
                        }
                    }
                } else {
                    // Default: break on any change
                    if (oldValue != newValue) {
                        return true;
                    }
                }
            }
        }
        return false;
    }
    
    void listBreakpoints() {
        if (breakpoints.empty()) {
            std::cout << "📭 No breakpoints set." << std::endl;
            return;
        }
        
        std::cout << "\n🔴 === ENHANCED BREAKPOINTS [" << breakpoints.size() << "] ===" << std::endl;
        for (const auto& [line, bp] : breakpoints) {
            std::cout << "  📍 Line " << line 
                      << " [" << (bp.enabled ? "enabled" : "disabled") << "]"
                      << " hits: " << bp.hitCount
                      << " @" << bp.simulatedAddress;
            
            if (!bp.condition.empty()) {
                std::cout << " condition: " << bp.condition;
            }
            if (!bp.watchVariable.empty()) {
                std::cout << " watching: " << bp.watchVariable;
            }
            if (!bp.memoryCondition.empty()) {
                std::cout << " memory condition: " << bp.memoryCondition;
            }
            std::cout << std::endl;
        }
    }
    
    void hitBreakpoint(int line) {
        auto it = breakpoints.find(line);
        if (it != breakpoints.end()) {
            it->second.hitCount++;
        }
    }
    
    void toggleBreakpoint(int line) {
        auto it = breakpoints.find(line);
        if (it != breakpoints.end()) {
            it->second.enabled = !it->second.enabled;
            std::cout << "🔴 Breakpoint at line " << line 
                      << (it->second.enabled ? " enabled" : " disabled") << std::endl;
        }
    }
};

// Memory-Aware Variable Tracker
class MemoryAwareVariableTracker {
private:
    std::map<std::string, EnhancedVariableInfo> globalVariables;
    std::vector<std::map<std::string, EnhancedVariableInfo>> scopeStack;
    std::vector<std::string> watchedVariables;
    bool realTimeMonitoring;
    std::function<void(const std::string&, const std::string&, const std::string&)> variableChangeCallback;
    
public:
    MemoryAwareVariableTracker() : realTimeMonitoring(true) {}
    
    void enableRealTimeMonitoring(bool enable) {
        realTimeMonitoring = enable;
        std::cout << "🔄 Real-time variable monitoring " 
                  << (enable ? "enabled" : "disabled") << std::endl;
    }
    
    void setVariableChangeCallback(std::function<void(const std::string&, const std::string&, const std::string&)> callback) {
        variableChangeCallback = callback;
    }
    
    void addToWatchList(const std::string& varName) {
        watchedVariables.push_back(varName);
        std::cout << "👁️ Added '" << varName << "' to watch list" << std::endl;
    }
    
    void removeFromWatchList(const std::string& varName) {
        auto it = std::find(watchedVariables.begin(), watchedVariables.end(), varName);
        if (it != watchedVariables.end()) {
            watchedVariables.erase(it);
            std::cout << "❌ Removed '" << varName << "' from watch list" << std::endl;
        }
    }
    
    void addVariable(const std::string& name, const std::string& value, 
                    const std::string& scope = "global", int line = 0) {
        EnhancedVariableInfo* existingVar = getVariableInfo(name);
        std::string oldValue = existingVar ? existingVar->value : "";
        
        if (existingVar) {
            // Variable exists, update it
            existingVar->updateValue(value, line);
            
            if (realTimeMonitoring) {
                std::cout << "🔄 Variable UPDATED: " << name 
                          << " = '" << value << "' (was: '" << oldValue << "')"
                          << " [" << existingVar->getDetailedTypeInfo() << "] "
                          << existingVar->getMemoryInfo()
                          << " (scope: " << scope << ", line: " << line << ")" << std::endl;
                
                showEnhancedVariableInfo(*existingVar);
            }
        } else {
            // New variable
            EnhancedVariableInfo var(name, value, scope);
            var.lastModifiedLine = line;
            
            if (scope == "global") {
                globalVariables[name] = var;
            } else if (!scopeStack.empty()) {
                scopeStack.back()[name] = var;
            }
            
            if (realTimeMonitoring) {
                std::cout << "✨ Variable CREATED: " << name 
                          << " = '" << value << "'"
                          << " [" << var.getDetailedTypeInfo() << "] "
                          << var.getMemoryInfo()
                          << " (scope: " << scope << ", line: " << line << ")" << std::endl;
                
                showEnhancedVariableInfo(var);
            }
        }
        
        // Check for variable watch breakpoints
        if (variableChangeCallback) {
            variableChangeCallback(name, oldValue, value);
        }
    }
    
    EnhancedVariableInfo* getVariableInfo(const std::string& name) {
        // Check local scope first (top of stack)
        if (!scopeStack.empty()) {
            auto& localScope = scopeStack.back();
            auto it = localScope.find(name);
            if (it != localScope.end()) {
                return &it->second;
            }
        }
        
        // Check global scope
        auto it = globalVariables.find(name);
        if (it != globalVariables.end()) {
            return &it->second;
        }
        
        return nullptr;
    }
    
    void pushScope() {
        scopeStack.push_back(std::map<std::string, EnhancedVariableInfo>());
        std::cout << "🔽 Pushed new scope (depth: " << scopeStack.size() << ")" << std::endl;
    }
    
    void popScope() {
        if (!scopeStack.empty()) {
            std::cout << "🔼 Popped scope (depth: " << scopeStack.size() << ")" << std::endl;
            scopeStack.pop_back();
        }
    }
    
    void showEnhancedVariableInfo(const EnhancedVariableInfo& var) {
        if (var.isList && !var.listElements.empty()) {
            std::cout << "   📋 List elements (" << var.listElements.size() << "): ";
            for (size_t i = 0; i < std::min(var.listElements.size(), size_t(3)); i++) {
                std::cout << "[" << i << "]=" << var.listElements[i] << " ";
            }
            if (var.listElements.size() > 3) {
                std::cout << "... (+" << (var.listElements.size() - 3) << " more)";
            }
            std::cout << std::endl;
        }
        
        if (var.isDictionary && !var.dictElements.empty()) {
            std::cout << "   📚 Dict elements (" << var.dictElements.size() << "): ";
            int count = 0;
            for (const auto& [key, value] : var.dictElements) {
                if (count >= 2) {
                    std::cout << "... (+" << (var.dictElements.size() - 2) << " more)";
                    break;
                }
                std::cout << key << "=" << value << " ";
                count++;
            }
            std::cout << std::endl;
        }
        
        if (var.isArray && !var.arrayElements.empty()) {
            std::cout << "   🗂️ Array elements (" << var.arrayElements.size() << ")" << std::endl;
        }
    }
    
    void showMemoryAnalysis(const std::string& varName) {
        EnhancedVariableInfo* var = getVariableInfo(varName);
        if (!var) {
            std::cout << "❌ Variable '" << varName << "' not found!" << std::endl;
            return;
        }
        
        std::cout << "\n🧠 === MEMORY ANALYSIS: " << varName << " ===" << std::endl;
        std::cout << "📍 Memory Address: " << var->simulatedAddress << std::endl;
        std::cout << "📏 Memory Size: " << var->estimatedSize << " bytes" << std::endl;
        std::cout << "🏷️ Type: " << var->getDetailedTypeInfo() << std::endl;
        std::cout << "📊 Ref Count: " << var->refCount << std::endl;
        std::cout << "🔢 Access Count: " << var->accessCount << std::endl;
        std::cout << "💾 Value: '" << var->value << "'" << std::endl;
        
        if (!var->previousValue.empty() && var->previousValue != var->value) {
            std::cout << "⏮️ Previous: '" << var->previousValue << "'" << std::endl;
        }
        
        if (!var->valueHistory.empty()) {
            std::cout << "📜 History (" << var->valueHistory.size() << "): ";
            for (size_t i = 0; i < std::min(var->valueHistory.size(), size_t(3)); i++) {
                if (i > 0) std::cout << " → ";
                std::cout << "'" << var->valueHistory[var->valueHistory.size() - 1 - i] << "'";
            }
            if (var->valueHistory.size() > 3) {
                std::cout << " ... (+" << (var->valueHistory.size() - 3) << " more)";
            }
            std::cout << std::endl;
        }
        
        if (!var->hexDump.empty()) {
            std::cout << "🔧 Hex Dump:" << std::endl;
            std::cout << var->hexDump << std::endl;
        }
        
        // Show type-specific analysis
        showTypeSpecificAnalysis(*var);
    }
    
    void showTypeSpecificAnalysis(const EnhancedVariableInfo& var) {
        if (var.isList && !var.listElements.empty()) {
            std::cout << "\n📋 LIST ANALYSIS:" << std::endl;
            std::cout << "  📏 Length: " << var.listElements.size() << " elements" << std::endl;
            for (size_t i = 0; i < std::min(var.listElements.size(), size_t(5)); i++) {
                void* elemAddr = reinterpret_cast<void*>(0x30000000 + i * 0x1000);
                std::cout << "  [" << i << "] = '" << var.listElements[i] 
                          << "' @" << elemAddr << std::endl;
            }
            if (var.listElements.size() > 5) {
                std::cout << "  ... (+" << (var.listElements.size() - 5) << " more)" << std::endl;
            }
        }
        
        if (var.isDictionary && !var.dictElements.empty()) {
            std::cout << "\n📚 DICTIONARY ANALYSIS:" << std::endl;
            std::cout << "  📏 Size: " << var.dictElements.size() << " key-value pairs" << std::endl;
            int count = 0;
            for (const auto& [key, value] : var.dictElements) {
                if (count >= 5) {
                    std::cout << "  ... (+" << (var.dictElements.size() - 5) << " more)" << std::endl;
                    break;
                }
                void* valueAddr = reinterpret_cast<void*>(0x40000000 + count * 0x1000);
                std::cout << "  ['" << key << "'] = '" << value 
                          << "' @" << valueAddr << std::endl;
                count++;
            }
        }
    }
    
    void listVariables() {
        std::cout << "\n📊 === ENHANCED VARIABLE DISPLAY ===" << std::endl;
        
        // Show local variables if in a function
        if (!scopeStack.empty() && !scopeStack.back().empty()) {
            std::cout << "🔹 Local Variables:" << std::endl;
            for (const auto& [name, var] : scopeStack.back()) {
                displayVariable(var, "  ");
            }
        }
        
        // Show global variables
        if (!globalVariables.empty()) {
            std::cout << "🌐 Global Variables:" << std::endl;
            for (const auto& [name, var] : globalVariables) {
                displayVariable(var, "  ");
            }
        }
        
        if (globalVariables.empty() && (scopeStack.empty() || scopeStack.back().empty())) {
            std::cout << "  📭 No variables defined." << std::endl;
        }
        
        // Show watch list
        if (!watchedVariables.empty()) {
            std::cout << "\n👁️ Watched Variables:" << std::endl;
            for (const auto& watchedVar : watchedVariables) {
                EnhancedVariableInfo* info = getVariableInfo(watchedVar);
                if (info) {
                    displayVariable(*info, "  ", true);
                } else {
                    std::cout << "  ❓ " << watchedVar << " = <undefined>" << std::endl;
                }
            }
        }
        
        // Show summary statistics
        showVariableStatistics();
    }
    
private:
    void displayVariable(const EnhancedVariableInfo& var, const std::string& indent, bool isWatched = false) {
        std::string prefix = isWatched ? "👁️ " : "";
        std::string typeIcon = var.getTypeIcon();
        
        std::cout << indent << prefix << typeIcon << " " << var.name 
                  << " = '" << var.value << "' [" << var.getDetailedTypeInfo() << "] "
                  << var.getMemoryInfo()
                  << " (line: " << var.lastModifiedLine 
                  << ", accessed: " << var.accessCount << "x)" << std::endl;
        
        // Show previous value if different
        if (!var.previousValue.empty() && var.previousValue != var.value) {
            std::cout << indent << "    ↳ Previous: '" << var.previousValue << "'" << std::endl;
        }
        
        // Show brief structure info for complex types
        if (var.isList && !var.listElements.empty()) {
            std::cout << indent << "    📋 " << var.listElements.size() << " elements";
            if (var.listElements.size() <= 3) {
                std::cout << ": " << var.listElements[0];
                for (size_t i = 1; i < var.listElements.size(); i++) {
                    std::cout << ", " << var.listElements[i];
                }
            }
            std::cout << std::endl;
        }
        
        if (var.isDictionary && !var.dictElements.empty()) {
            std::cout << indent << "    📚 " << var.dictElements.size() << " pairs";
            if (var.dictElements.size() <= 2) {
                bool first = true;
                for (const auto& [key, value] : var.dictElements) {
                    if (!first) std::cout << ", ";
                    std::cout << ": " << key << "=" << value;
                    first = false;
                }
            }
            std::cout << std::endl;
        }
    }
    
    void showVariableStatistics() {
        int integers = 0, floats = 0, strings = 0, lists = 0, dictionaries = 0, arrays = 0, empty = 0;
        size_t totalMemory = 0;
        
        auto countVar = [&](const EnhancedVariableInfo& var) {
            if (var.type == "integer") integers++;
            else if (var.type == "float") floats++;
            else if (var.type == "string") strings++;
            else if (var.type == "list") lists++;
            else if (var.type == "dictionary") dictionaries++;
            else if (var.type == "array") arrays++;
            else if (var.type == "empty") empty++;
            totalMemory += var.estimatedSize;
        };
        
        for (const auto& [name, var] : globalVariables) {
            countVar(var);
        }
        
        for (const auto& scope : scopeStack) {
            for (const auto& [name, var] : scope) {
                countVar(var);
            }
        }
        
        std::cout << "\n📊 Variable Statistics:" << std::endl;
        if (integers > 0) std::cout << "  🔢 " << integers << " integers ";
        if (floats > 0) std::cout << "🔣 " << floats << " floats ";
        if (strings > 0) std::cout << "📝 " << strings << " strings ";
        if (lists > 0) std::cout << "📋 " << lists << " lists ";
        if (dictionaries > 0) std::cout << "📚 " << dictionaries << " dictionaries ";
        if (arrays > 0) std::cout << "🗂️ " << arrays << " arrays ";
        if (empty > 0) std::cout << "🗳️ " << empty << " empty ";
        std::cout << std::endl;
        std::cout << "  💾 Total estimated memory: " << totalMemory << " bytes" << std::endl;
    }
};

// Continue with the rest of the implementation in the next part...
