# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

XenoARM JIT is a Just-In-Time compiler that translates Original Xbox x86 instructions to ARM AArch64 code. It's designed for real-time emulation of Xbox games on ARM devices and integrates with a Rust-based Xbox emulator via FFI.

## Build Commands

### Initial Setup
```bash
# Create build directory
mkdir -p build
cd build

# Configure with CMake (Debug mode with tests)
cmake -DCMAKE_BUILD_TYPE=Debug -DXENOARM_BUILD_TESTS=ON ..

# Configure with sanitizers for memory debugging
cmake -DCMAKE_BUILD_TYPE=Debug -DXENOARM_ENABLE_SANITIZERS=ON ..

# Build the project
cmake --build .
```

### Running Tests
```bash
# Run all tests
cd build && ctest --output-on-failure --verbose

# Run specific test suite
./tests/unit_tests --gtest_filter="DecoderTest.*"

# Run with detailed output
./tests/unit_tests --gtest_print_time=1 --gtest_color=yes

# Generate test report
./tests/unit_tests --gtest_output=xml:test_results.xml
```

### Development Commands
```bash
# Clean build
cmake --build . --target clean

# Format code (if clang-format configured)
find src include tests -name "*.cpp" -o -name "*.h" | xargs clang-format -i

# Run specific decoder tests
./tests/unit_tests --gtest_filter="Decoder*"

# Run memory interface tests
./tests/unit_tests --gtest_filter="*Memory*"
```

## Architecture Overview

The JIT follows a pipeline architecture with clear phase separation:

1. **x86 Decoder** (`src/decoder/`) - Parses x86 instructions into internal representation
   - Handles prefixes, ModR/M, SIB, displacements, immediates
   - Supports x86, SSE, MMX, and basic x87 instructions
   - Modular opcode handlers for extensibility

2. **IR Builder** (`src/ir/`) - Converts decoded x86 to intermediate representation
   - SSA form with phi nodes for control flow joins
   - Type-safe IR operations
   - Preserves x86 semantics including flags

3. **Optimizer** (`src/optimizer/`) - Performs optimization passes on IR
   - Dead code elimination
   - Constant folding
   - Flag optimization

4. **Code Generator** (`src/codegen/`) - Translates IR to ARM64 instructions
   - Register allocation with x86→ARM mapping
   - Memory model translation (x86 TSO → ARM acquire-release)
   - SSE→NEON instruction mapping

5. **Translation Cache** (`src/cache/`) - Stores and manages compiled code blocks
   - Fast lookup with hash table + LRU eviction
   - Block linking for hot paths
   - Self-modifying code detection

6. **Runtime System** (`src/runtime/`) - Manages execution state
   - CPU state management (registers, flags)
   - Memory interface with callbacks
   - Exception handling

## Critical Development Guidelines

### Test-Driven Development (MANDATORY - STRICTLY ENFORCED)

**ABSOLUTE RULES - NO EXCEPTIONS:**

1. **RED-GREEN-REFACTOR ONLY**
   - RED: Write a failing test FIRST (test must fail)
   - GREEN: Write MINIMAL code to make test pass
   - REFACTOR: Clean up code while keeping tests green
   - **VIOLATION**: Writing ANY implementation code before a failing test

2. **TEST IMMUTABILITY**
   - Tests are CONTRACTS - once written, they CANNOT be changed
   - If a test seems wrong, the implementation is wrong, NOT the test
   - **VIOLATION**: Modifying any test to make implementation pass
   - **EXCEPTION**: Only if requirements genuinely changed (must document why)

3. **NO IMPLEMENTATION WITHOUT TESTS**
   - Every line of implementation code MUST have a corresponding test
   - No "temporary" code without tests
   - No "quick fixes" without tests
   - **VIOLATION**: Any implementation code not covered by tests

4. **TEST-FIRST WORKFLOW**
   ```
   1. Study existing interfaces/structures
   2. Write failing test
   3. Run test - MUST see it fail
   4. Write minimal implementation
   5. Run test - MUST see it pass
   6. Refactor if needed
   7. Run test - MUST still pass
   ```

5. **ENFORCEMENT CHECKS**
   - Before ANY implementation: "Have I written a failing test?"
   - Before ANY commit: "Are all tests passing?"
   - Before ANY PR: "Is every line covered by tests?"
   - If answer is NO to any: STOP and write tests first

6. **TDD VIOLATIONS TO AVOID**
   - ❌ Writing implementation then tests
   - ❌ Modifying tests to match implementation
   - ❌ Skipping tests for "simple" code
   - ❌ Writing multiple features before testing
   - ❌ Commenting out failing tests
   - ❌ Writing tests that always pass

7. **CORRECT TDD EXAMPLE**
   ```cpp
   // STEP 1: Write failing test
   TEST(DecoderTest, DecodeADD) {
       uint8_t code[] = {0x01, 0xD8}; // add eax, ebx
       auto inst = decoder.decode(code, 2);
       EXPECT_EQ(inst.opcode, Opcode::ADD);  // This MUST fail first
   }
   
   // STEP 2: Run test, see failure
   // STEP 3: Write minimal implementation
   // STEP 4: Test passes
   // STEP 5: Refactor if needed
   ```

### Current Development Status
- 🔴 Phase 1: Foundation (NOT STARTED) - Need to create basic infrastructure, logging, memory interface
- 🔴 Phase 2: x86 Decoder (NOT STARTED) - Need to implement instruction decoding  
- 🔴 Phase 3: IR System (NOT STARTED) - Need to implement IR translation
- 🔴 Phase 4: ARM Code Generation (NOT STARTED)
- 🔴 Phase 5: Runtime System (NOT STARTED)
- 🔴 Phase 6: Advanced Optimization (NOT STARTED)
- 🔴 Phase 7: API and Integration (NOT STARTED)
- 🔴 Phase 8: Validation & Hardening (NOT STARTED)

**PROJECT STATE**: This is a new project with only documentation. No implementation code exists yet. All development must follow strict TDD principles from the beginning.

### Memory Interface Pattern
The JIT uses callbacks for memory access to integrate with the emulator:
```cpp
// Memory callbacks are provided by the emulator
struct MemoryCallbacks {
    uint8_t (*read_byte)(void* context, uint32_t address);
    void (*write_byte)(void* context, uint32_t address, uint8_t value);
    // ... other sizes
};
```

### Register Allocation Strategy
```
X0-X18:  Temporary registers for computation
X19-X26: Fixed mappings for x86 registers
X27:     CPU state pointer
X28:     Translation cache base
X29:     Frame pointer
X30:     Link register
```

### Code Style
- C++17 or newer
- Classes: PascalCase
- Functions: camelCase  
- Variables: snake_case
- Constants: UPPER_SNAKE_CASE
- Namespaces: lowercase

### Performance Requirements
- Translation: < 1000 cycles per x86 instruction
- Execution: 5-10x slower than native
- Cache hit rate: > 95% for typical games
- Memory overhead: < 100MB for translation cache