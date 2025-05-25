# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

XenoARM JIT is a high-performance JIT compiler that translates Original Xbox x86 instructions (Intel Pentium III, 32-bit IA-32) to ARM AArch64 machine code. It's designed to be integrated into a Rust-based Xbox emulator via C FFI.

## Build Commands

```bash
# Configure build (from project root)
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Build
make -j$(nproc)

# Build with sanitizers
cmake -DCMAKE_BUILD_TYPE=Debug -DXENOARM_ENABLE_SANITIZERS=ON ..
make -j$(nproc)

# Release build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)
```

## Testing Commands

```bash
# Run all tests
ctest --output-on-failure

# Run specific test suite
./tests/unit_tests --gtest_filter="DecoderTest.*"

# Run with verbose output
ctest --verbose --output-on-failure

# Run specific test binary
./tests/unit_tests
./tests/integration_tests

# Generate coverage report (Debug build only)
make coverage
```

## Development Workflow

The project follows Test-Driven Development (TDD):
1. Write failing test in `tests/unit/` or `tests/integration/`
2. Implement minimal code to pass test
3. Refactor while keeping tests green

## Current Implementation Status

### Phase 1: Foundation ✅ COMPLETE
- ✅ Directory structure created
- ✅ Core utilities implemented (logging, bit_utils, memory_utils, error handling)
- ✅ Memory interface and C API adapter implemented
- ✅ CPU state structure defined
- ✅ Build system configured with CMake + Google Test
- ✅ Memory interface tests implemented and passing (8/8 tests)

**Files implemented:**
- `src/utils/common.h` - Core types and constants
- `src/utils/logging.h/cpp` - Thread-safe logging system
- `src/utils/bit_utils.h` - Bit manipulation utilities
- `src/utils/memory_utils.h` - Memory management utilities
- `src/utils/error.h` - Error handling infrastructure
- `src/runtime/memory_interface.h` - IMemoryInterface abstract class
- `src/runtime/cpu_state.h` - CPU state structure with x86 layout
- `src/api/memory_callback_adapter.h/cpp` - C callback to C++ adapter
- `include/xenoarm/xenoarm_jit.h` - C API header for FFI
- `tests/unit/utils/test_memory_interface.cpp` - Memory interface tests

### Phase 2: x86 Decoder 🔄 IN PROGRESS
**Completed TDD cycles:**
1. ✅ **Basic Decoder Tests**: Created failing tests for MOV and ADD instruction decoding
2. ✅ **Minimal Implementation**: Implemented just enough code to make tests pass
   - MOV register-to-register (0x89)
   - MOV immediate to EAX (0xB8) 
   - ADD register-to-register (0x01)
   - Proper error handling for invalid opcodes and memory bounds
3. ✅ **ModR/M Byte Parsing**: Proper parsing of ModR/M bytes
   - Correct extraction of mod, reg, and r/m fields
   - Register-to-register operand handling
   - Memory operand handling with displacements (0, 8-bit, 32-bit)
   - Proper instruction length calculation
4. ✅ **Arithmetic Instructions**: Extended decoder for more x86 arithmetic
   - SUB, XOR, CMP, AND, OR register-to-register operations
   - ADD/SUB immediate to EAX operations
   - Bidirectional opcodes (0x8B MOV r32,r/m32, 0x03 ADD r32,r/m32, etc.)
5. ✅ **SIB Byte Addressing**: Full SIB byte support
   - Scale-Index-Base addressing with scales 1, 2, 4, 8
   - Special cases: ESP cannot be index, base=5 with mod=0
   - SIB with all displacement sizes (0, 8-bit, 32-bit)
6. ✅ **Prefix Handling**: Complete prefix support
   - Segment override prefixes (ES, CS, SS, DS, FS, GS)
   - LOCK prefix for atomic operations
   - REP/REPNE prefixes for string operations
   - Operand-size override (0x66) for 16-bit operations
   - Address-size override (0x67) for 16-bit addressing
   - Multiple prefix combinations
   - String operations: MOVS, SCAS
7. ✅ **Control Flow Instructions**: Complete control flow support
   - JMP short (EB) and near (E9) with relative addressing
   - JMP indirect through register/memory (FF /4)
   - Conditional jumps (Jcc) short (70-7F) and near (0F 80-8F)
   - CALL near (E8) and indirect (FF /2)
   - RET (C3) and RET with immediate (C2)
   - LOOP/LOOPE/LOOPNE instructions (E0-E2)
   - Special case handling: [disp32] addressing with mod=00, r/m=101

**Files implemented:**
- `src/decoder/instruction.h` - Instruction structure with opcodes, operands, and prefix support
- `src/decoder/decoder.h` - IDecoder interface and basic Decoder class
- `src/decoder/decoder.cpp` - Decoder with ModR/M, SIB, prefix, and control flow parsing
- `tests/unit/decoder/test_basic_decoder.cpp` - Basic decoder tests
- `tests/unit/decoder/test_modrm_parsing.cpp` - ModR/M parsing tests
- `tests/unit/decoder/test_arithmetic_instructions.cpp` - Arithmetic instruction tests
- `tests/unit/decoder/test_sib_addressing.cpp` - SIB byte tests
- `tests/unit/decoder/test_prefix_handling.cpp` - Prefix handling tests
- `tests/unit/decoder/test_control_flow_instructions.cpp` - Control flow tests

**Current test status: 58/58 tests passing** 
- 6 memory interface tests
- 5 basic decoder tests
- 5 ModR/M parsing tests
- 8 arithmetic instruction tests
- 6 SIB addressing tests
- 12 prefix handling tests
- 14 control flow tests
- 2 infrastructure tests

**Next TDD cycle:**
1. Extend to SSE/MMX/x87 instructions
2. Implement instruction block decoding
3. Add more complex addressing modes
4. Add push/pop instructions

**NOTE**: Successfully following TDD - wrote failing tests FIRST, then implemented minimal code to pass.

## Architecture Overview

The JIT compiler pipeline consists of these major components:

1. **Decoder** (`src/decoder/`) - Parses x86 instruction bytes into structured format
2. **IR Builder** (`src/ir/`) - Converts x86 instructions to SSA-form intermediate representation
3. **Optimizer** (`src/optimizer/`) - Performs optimization passes (dead code elimination, constant folding, flag optimization)
4. **Code Generator** (`src/codegen/`) - Translates IR to ARM AArch64 instructions
5. **Translation Cache** (`src/cache/`) - Stores and manages compiled code blocks
6. **Dispatcher** (`src/runtime/`) - Executes translated code and manages CPU state

Key architectural decisions:
- Memory ordering: x86 TSO emulated using ARM acquire-release semantics
- Register allocation: x86 registers mapped to ARM X19-X26, with X0-X18 for temporaries
- Flag handling: Lazy evaluation with on-demand calculation for complex flags
- Basic blocks: Maximum 100 instructions per translation unit
- Cache: 32-64MB with LRU eviction

## C API for Rust Integration

The project provides a C-compatible API (`include/xenoarm/xenoarm_jit.h`) designed for safe FFI with Rust:
- Opaque pointers for all internal structures
- Callback-based memory interface
- Error codes instead of exceptions at API boundary
- Thread safety: Each thread needs its own JIT instance

## Code Style

- C++17 standard
- Naming: PascalCase for classes, camelCase for functions, snake_case for variables
- Namespaces: `xenoarm::` for all code, with sub-namespaces for major components
- Modern C++ idioms: smart pointers, RAII, move semantics

## Critical Implementation Notes

1. **Memory Model**: ARM requires explicit cache synchronization for self-modifying code, unlike x86
2. **SIMD**: SSE instructions map to NEON using patterns from `docs/architecture/sse-to-neon-mapping.md`
3. **Performance**: Target 5-10x slower than native, with >95% translation cache hit rate
4. **Testing**: All x86 instruction implementations must have corresponding unit tests

When implementing new features:
- Check component interfaces in `docs/architecture/component-interfaces.md`
- Follow IR specification in `docs/architecture/ir-specification.md`
- Refer to ARM code patterns in `docs/architecture/arm-aarch64-reference.md`
- Ensure memory ordering correctness per `docs/architecture/memory-model.md`