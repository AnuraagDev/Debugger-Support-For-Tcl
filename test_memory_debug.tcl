#!/usr/bin/env tclsh
# Test script for memory-level debugging

puts "🧪 Testing TCL Memory Debugger"
puts "================================"

# Test different data types
set mystring "Hello, World!"
set mynumber 42
set myfloat 3.14159

# Test list
set mylist {item1 item2 item3 "item with spaces"}
puts "List created: $mylist"

# Test dictionary
set mydict {
    name "John Doe"
    age 30
    city "New York"
    hobbies {reading coding traveling}
}
puts "Dictionary created"

# Test nested structures
set complex_data {
    users {
        {id 1 name "Alice" scores {85 92 78}}
        {id 2 name "Bob" scores {76 88 91}}
    }
    metadata {
        version "1.0"
        created "2025-01-01"
    }
}

# Test array
array set myarray {
    key1 "value1"
    key2 "value2"
    key3 "value3"
}

# Test variable modifications
puts "\n🔄 Testing variable modifications..."
set counter 0
for {set i 0} {$i < 5} {incr i} {
    incr counter
    puts "Counter: $counter"
}

# Test procedure with local variables
proc test_procedure {param1 param2} {
    set local_var "I am local"
    set result [expr $param1 + $param2]
    puts "Procedure result: $result"
    return $result
}

set proc_result [test_procedure 10 20]

# Test namespace
namespace eval myspace {
    set ns_var "I am in a namespace"
    
    proc ns_proc {} {
        variable ns_var
        puts "Namespace variable: $ns_var"
    }
}

myspace::ns_proc

puts "\n✅ All tests completed!"
puts "Variables that should be tracked:"
puts "  - mystring (string)"
puts "  - mynumber (integer)"  
puts "  - myfloat (float)"
puts "  - mylist (list)"
puts "  - mydict (dictionary)"
puts "  - complex_data (nested structure)"
puts "  - myarray (array)"
puts "  - counter (modified multiple times)"
