#include "decoder.h"
#include "../utils/logging.h"
#include <algorithm>

namespace xenoarm {
namespace x86 {

size_t Decoder::decodeInstruction(IMemoryInterface* memory, 
                                 uint32_t pc, 
                                 Instruction& out_instruction) {
    if (!memory) {
        LOG_DEBUG("Memory interface is null");
        return 0;
    }
    
    // Reset instruction
    out_instruction = Instruction{};
    out_instruction.address = pc;
    
    uint32_t current_pc = pc;
    uint8_t byte;
    
    // Parse prefixes
    bool done_prefixes = false;
    while (!done_prefixes) {
        if (!memory->readMemory(current_pc, &byte, 1)) {
            LOG_DEBUG("Failed to read byte at 0x%08x", current_pc);
            return 0; // Error reading memory
        }
        
        switch (byte) {
            // Segment override prefixes
            case 0x26: out_instruction.segment_override = SegmentRegister::ES; current_pc++; break;
            case 0x2E: out_instruction.segment_override = SegmentRegister::CS; current_pc++; break;
            case 0x36: out_instruction.segment_override = SegmentRegister::SS; current_pc++; break;
            case 0x3E: out_instruction.segment_override = SegmentRegister::DS; current_pc++; break;
            case 0x64: out_instruction.segment_override = SegmentRegister::FS; current_pc++; break;
            case 0x65: out_instruction.segment_override = SegmentRegister::GS; current_pc++; break;
            
            // Operand-size override
            case 0x66: 
                out_instruction.has_operand_size_override = true;
                out_instruction.operand_size = OperandSize::Word;
                current_pc++;
                break;
                
            // Address-size override
            case 0x67:
                out_instruction.has_address_size_override = true;
                out_instruction.address_size = AddressSize::Addr16;
                current_pc++;
                break;
                
            // LOCK prefix
            case 0xF0:
                out_instruction.has_lock_prefix = true;
                current_pc++;
                break;
                
            // REP/REPE prefix
            case 0xF3:
                out_instruction.has_rep_prefix = true;
                current_pc++;
                break;
                
            // REPNE prefix
            case 0xF2:
                out_instruction.has_repne_prefix = true;
                current_pc++;
                break;
                
            default:
                done_prefixes = true;
                break;
        }
    }
    
    // Now byte contains the actual opcode
    uint8_t opcode = byte;
    
    // Basic decoding implementation
    switch (opcode) {
        case 0x89: // MOV r/m32, r32
        case 0x8B: // MOV r32, r/m32
        case 0x01: // ADD r/m32, r32
        case 0x03: // ADD r32, r/m32
        case 0x29: // SUB r/m32, r32
        case 0x2B: // SUB r32, r/m32
        case 0x31: // XOR r/m32, r32
        case 0x39: // CMP r/m32, r32
        case 0x21: // AND r/m32, r32
        case 0x09: // OR r/m32, r32
            {
                uint8_t modrm;
                if (!memory->readMemory(current_pc + 1, &modrm, 1)) {
                    return 0; // Error reading ModR/M
                }
                
                // Set opcode based on instruction
                switch (opcode) {
                    case 0x89: 
                    case 0x8B: out_instruction.opcode = Instruction::Opcode::MOV; break;
                    case 0x01: 
                    case 0x03: out_instruction.opcode = Instruction::Opcode::ADD; break;
                    case 0x29: 
                    case 0x2B: out_instruction.opcode = Instruction::Opcode::SUB; break;
                    case 0x31: out_instruction.opcode = Instruction::Opcode::XOR; break;
                    case 0x39: out_instruction.opcode = Instruction::Opcode::CMP; break;
                    case 0x21: out_instruction.opcode = Instruction::Opcode::AND; break;
                    case 0x09: out_instruction.opcode = Instruction::Opcode::OR; break;
                }
                
                // Parse ModR/M and get instruction length
                size_t total_length = decodeModRM(memory, current_pc, out_instruction);
                if (total_length == 0) {
                    return 0; // Error parsing ModR/M
                }
                
                // Add prefix length to total
                total_length += current_pc - pc;
                
                // For opcodes with reversed direction (r32, r/m32), swap operands
                if (opcode == 0x8B || opcode == 0x03 || opcode == 0x2B) {
                    std::swap(out_instruction.src, out_instruction.dst);
                }
                
                out_instruction.length = total_length;
                return total_length;
            }
            
        case 0xB8: // MOV EAX, imm32  
            {
                uint32_t immediate;
                if (!memory->readMemory(current_pc + 1, &immediate, 4)) {
                    return 0; // Error reading immediate
                }
                
                out_instruction.opcode = Instruction::Opcode::MOV;
                out_instruction.length = 5 + (current_pc - pc);
                // TODO: Set up operands properly for immediate MOV
                return out_instruction.length;
            }
            
        case 0x05: // ADD EAX, imm32
        case 0x2D: // SUB EAX, imm32
            {
                uint32_t immediate;
                if (!memory->readMemory(current_pc + 1, &immediate, 4)) {
                    return 0; // Error reading immediate
                }
                
                out_instruction.opcode = (opcode == 0x05) ? Instruction::Opcode::ADD : Instruction::Opcode::SUB;
                out_instruction.dst.type = Instruction::Operand::Type::REGISTER;
                out_instruction.dst.reg = 0; // EAX
                out_instruction.src.type = Instruction::Operand::Type::IMMEDIATE;
                out_instruction.src.imm = immediate;
                out_instruction.length = 5 + (current_pc - pc);
                return out_instruction.length;
            }
            
        // String operations
        case 0xA5: // MOVS DWORD
            out_instruction.opcode = Instruction::Opcode::MOVS;
            out_instruction.length = 1 + (current_pc - pc);
            return out_instruction.length;
            
        case 0xAF: // SCAS DWORD
            out_instruction.opcode = Instruction::Opcode::SCAS;
            out_instruction.length = 1 + (current_pc - pc);
            return out_instruction.length;
            
        // Control flow instructions
        case 0xEB: // JMP short (8-bit relative)
            {
                int8_t rel8;
                if (!memory->readMemory(current_pc + 1, &rel8, 1)) {
                    return 0; // Error reading displacement
                }
                
                out_instruction.opcode = Instruction::Opcode::JMP;
                out_instruction.dst.type = Instruction::Operand::Type::IMMEDIATE;
                out_instruction.dst.imm = static_cast<uint32_t>(rel8);
                out_instruction.length = 2 + (current_pc - pc);
                return out_instruction.length;
            }
            
        case 0xE9: // JMP near (32-bit relative)
            {
                int32_t rel32;
                if (!memory->readMemory(current_pc + 1, &rel32, 4)) {
                    return 0; // Error reading displacement
                }
                
                out_instruction.opcode = Instruction::Opcode::JMP;
                out_instruction.dst.type = Instruction::Operand::Type::IMMEDIATE;
                out_instruction.dst.imm = static_cast<uint32_t>(rel32);
                out_instruction.length = 5 + (current_pc - pc);
                return out_instruction.length;
            }
            
        case 0xE8: // CALL near (32-bit relative)
            {
                int32_t rel32;
                if (!memory->readMemory(current_pc + 1, &rel32, 4)) {
                    return 0; // Error reading displacement
                }
                
                out_instruction.opcode = Instruction::Opcode::CALL;
                out_instruction.dst.type = Instruction::Operand::Type::IMMEDIATE;
                out_instruction.dst.imm = static_cast<uint32_t>(rel32);
                out_instruction.length = 5 + (current_pc - pc);
                return out_instruction.length;
            }
            
        case 0xC3: // RET
            out_instruction.opcode = Instruction::Opcode::RET;
            out_instruction.length = 1 + (current_pc - pc);
            return out_instruction.length;
            
        case 0xC2: // RET imm16
            {
                uint16_t imm16;
                if (!memory->readMemory(current_pc + 1, &imm16, 2)) {
                    return 0; // Error reading immediate
                }
                
                out_instruction.opcode = Instruction::Opcode::RET;
                out_instruction.src.type = Instruction::Operand::Type::IMMEDIATE;
                out_instruction.src.imm = imm16;
                out_instruction.length = 3 + (current_pc - pc);
                return out_instruction.length;
            }
            
        case 0x70: case 0x71: case 0x72: case 0x73: // Jcc short (conditional jumps)
        case 0x74: case 0x75: case 0x76: case 0x77:
        case 0x78: case 0x79: case 0x7A: case 0x7B:
        case 0x7C: case 0x7D: case 0x7E: case 0x7F:
            {
                int8_t rel8;
                if (!memory->readMemory(current_pc + 1, &rel8, 1)) {
                    return 0; // Error reading displacement
                }
                
                out_instruction.opcode = Instruction::Opcode::JCC;
                out_instruction.dst.type = Instruction::Operand::Type::IMMEDIATE;
                out_instruction.dst.imm = static_cast<uint32_t>(static_cast<int32_t>(rel8));
                out_instruction.length = 2 + (current_pc - pc);
                return out_instruction.length;
            }
            
        case 0xE0: // LOOPNE/LOOPNZ
        case 0xE1: // LOOPE/LOOPZ
        case 0xE2: // LOOP
            {
                int8_t rel8;
                if (!memory->readMemory(current_pc + 1, &rel8, 1)) {
                    return 0; // Error reading displacement
                }
                
                out_instruction.opcode = Instruction::Opcode::LOOP;
                out_instruction.dst.type = Instruction::Operand::Type::IMMEDIATE;
                out_instruction.dst.imm = static_cast<uint32_t>(static_cast<int32_t>(rel8));
                out_instruction.length = 2 + (current_pc - pc);
                return out_instruction.length;
            }
            
        case 0xFF: // Group 5 - JMP/CALL indirect
            {
                uint8_t modrm;
                if (!memory->readMemory(current_pc + 1, &modrm, 1)) {
                    return 0; // Error reading ModR/M
                }
                
                uint8_t reg = (modrm >> 3) & 0x07;  // Bits 5-3
                
                // Check which operation based on reg field
                switch (reg) {
                    case 2: // CALL indirect
                        out_instruction.opcode = Instruction::Opcode::CALL;
                        break;
                    case 4: // JMP indirect
                        out_instruction.opcode = Instruction::Opcode::JMP;
                        break;
                    default:
                        LOG_DEBUG("Unknown FF group opcode with reg=%d at 0x%08x", reg, current_pc);
                        return 0;
                }
                
                // Parse ModR/M to get the operand
                size_t total_length = decodeModRM(memory, current_pc, out_instruction);
                if (total_length == 0) {
                    return 0; // Error parsing ModR/M
                }
                
                // For FF group, the operand is in dst (r/m field), not src (reg field)
                // decodeModRM already set it up correctly in dst, so we just clear src
                out_instruction.src = Instruction::Operand{};
                
                // Add prefix length to total
                total_length += current_pc - pc;
                out_instruction.length = total_length;
                return total_length;
            }
            
        case 0x0F: // Two-byte opcodes
            {
                uint8_t second_byte;
                if (!memory->readMemory(current_pc + 1, &second_byte, 1)) {
                    return 0; // Error reading second opcode byte
                }
                
                // Handle Jcc near (0F 80-8F)
                if (second_byte >= 0x80 && second_byte <= 0x8F) {
                    int32_t rel32;
                    if (!memory->readMemory(current_pc + 2, &rel32, 4)) {
                        return 0; // Error reading displacement
                    }
                    
                    out_instruction.opcode = Instruction::Opcode::JCC;
                    out_instruction.dst.type = Instruction::Operand::Type::IMMEDIATE;
                    out_instruction.dst.imm = static_cast<uint32_t>(rel32);
                    out_instruction.length = 6 + (current_pc - pc);
                    return out_instruction.length;
                } else {
                    LOG_DEBUG("Unknown two-byte opcode 0F %02x at 0x%08x", second_byte, current_pc);
                    return 0;
                }
            }
            
        default:
            LOG_DEBUG("Unknown opcode 0x%02x at 0x%08x", opcode, current_pc);
            out_instruction.opcode = Instruction::Opcode::INVALID;
            return 0; // Invalid instruction
    }
}

std::vector<Instruction> Decoder::decodeBlock(IMemoryInterface* memory,
                                             uint32_t pc,
                                             size_t max_instructions) {
    // Placeholder implementation
    std::vector<Instruction> instructions;
    // Not implemented yet - will be done in later TDD cycles
    return instructions;
}

size_t Decoder::decodeOpcode(uint8_t opcode, IMemoryInterface* memory, uint32_t pc, Instruction& inst) {
    // Not implemented yet
    return 0;
}

size_t Decoder::decodeModRM(IMemoryInterface* memory, uint32_t pc, Instruction& inst) {
    uint8_t modrm;
    if (!memory->readMemory(pc + 1, &modrm, 1)) {
        return 0; // Error reading ModR/M
    }
    
    // Extract ModR/M fields
    uint8_t mod = (modrm >> 6) & 0x03;  // Bits 7-6
    uint8_t reg = (modrm >> 3) & 0x07;  // Bits 5-3 (source register)
    uint8_t rm  = modrm & 0x07;         // Bits 2-0 (destination)
    
    size_t total_length = 2; // opcode + modrm
    
    // Set up source operand (always a register for these opcodes)
    inst.src.type = Instruction::Operand::Type::REGISTER;
    inst.src.reg = reg;
    
    // Set up destination operand based on mod field
    switch (mod) {
        case 0x00: // Memory, no displacement (unless rm=101)
            if (rm == 4) { // SIB byte follows
                size_t sib_bytes = decodeSIB(memory, pc + 2, mod, inst.dst);
                if (sib_bytes == 0) {
                    return 0; // Error decoding SIB
                }
                total_length += sib_bytes;
            } else if (rm == 5) { // Special case: [disp32] with no base
                int32_t disp32;
                if (!memory->readMemory(pc + 2, &disp32, 4)) {
                    return 0; // Error reading displacement
                }
                
                inst.dst.type = Instruction::Operand::Type::MEMORY;
                inst.dst.mem.base = 0; // No base register
                inst.dst.mem.index = 0;
                inst.dst.mem.scale = 1;
                inst.dst.mem.displacement = disp32;
                inst.dst.mem.segment = 0;
                total_length = 6; // opcode + modrm + disp32
            } else {
                inst.dst.type = Instruction::Operand::Type::MEMORY;
                inst.dst.mem.base = rm;
                inst.dst.mem.index = 0;
                inst.dst.mem.scale = 1;
                inst.dst.mem.displacement = 0;
                inst.dst.mem.segment = 0;
            }
            break;
            
        case 0x01: // Memory + 8-bit displacement
            {
                if (rm == 4) { // SIB byte follows
                    size_t sib_bytes = decodeSIB(memory, pc + 2, mod, inst.dst);
                    if (sib_bytes == 0) {
                        return 0; // Error decoding SIB
                    }
                    total_length += sib_bytes;
                    
                    // Now read the 8-bit displacement
                    int8_t disp8;
                    if (!memory->readMemory(pc + 2 + sib_bytes, &disp8, 1)) {
                        return 0; // Error reading displacement
                    }
                    inst.dst.mem.displacement = static_cast<int32_t>(disp8);
                    total_length += 1; // disp8
                } else {
                    int8_t disp8;
                    if (!memory->readMemory(pc + 2, &disp8, 1)) {
                        return 0; // Error reading displacement
                    }
                    
                    inst.dst.type = Instruction::Operand::Type::MEMORY;
                    inst.dst.mem.base = rm;
                    inst.dst.mem.index = 0;
                    inst.dst.mem.scale = 1;
                    inst.dst.mem.displacement = static_cast<int32_t>(disp8);
                    inst.dst.mem.segment = 0;
                    total_length = 3; // opcode + modrm + disp8
                }
            }
            break;
            
        case 0x02: // Memory + 32-bit displacement
            {
                if (rm == 4) { // SIB byte follows
                    size_t sib_bytes = decodeSIB(memory, pc + 2, mod, inst.dst);
                    if (sib_bytes == 0) {
                        return 0; // Error decoding SIB
                    }
                    total_length += sib_bytes;
                    
                    // Now read the 32-bit displacement
                    int32_t disp32;
                    if (!memory->readMemory(pc + 2 + sib_bytes, &disp32, 4)) {
                        return 0; // Error reading displacement
                    }
                    inst.dst.mem.displacement = disp32;
                    total_length += 4; // disp32
                } else {
                    int32_t disp32;
                    if (!memory->readMemory(pc + 2, &disp32, 4)) {
                        return 0; // Error reading displacement
                    }
                    
                    inst.dst.type = Instruction::Operand::Type::MEMORY;
                    inst.dst.mem.base = rm;
                    inst.dst.mem.index = 0;
                    inst.dst.mem.scale = 1;
                    inst.dst.mem.displacement = disp32;
                    inst.dst.mem.segment = 0;
                    total_length = 6; // opcode + modrm + disp32
                }
            }
            break;
            
        case 0x03: // Register-to-register
            inst.dst.type = Instruction::Operand::Type::REGISTER;
            inst.dst.reg = rm;
            break;
            
        default:
            return 0; // Should never happen
    }
    
    inst.length = static_cast<uint8_t>(total_length);
    return total_length;
}

size_t Decoder::decodeSIB(IMemoryInterface* memory, uint32_t pc, uint8_t mod, Instruction::Operand& operand) {
    uint8_t sib;
    if (!memory->readMemory(pc, &sib, 1)) {
        return 0; // Error reading SIB
    }
    
    // Extract SIB fields
    uint8_t scale_bits = (sib >> 6) & 0x03;  // Bits 7-6
    uint8_t index = (sib >> 3) & 0x07;       // Bits 5-3
    uint8_t base = sib & 0x07;               // Bits 2-0
    
    // Convert scale bits to actual scale value
    uint8_t scale = 1 << scale_bits; // 1, 2, 4, or 8
    
    // Set operand type to memory
    operand.type = Instruction::Operand::Type::MEMORY;
    operand.mem.scale = scale;
    operand.mem.segment = 0; // Default segment
    
    size_t sib_length = 1; // SIB byte itself
    
    // Handle base register
    if (base == 5 && mod == 0) {
        // Special case: no base register, 32-bit displacement follows
        operand.mem.base = 0; // No base register
        int32_t disp32;
        if (!memory->readMemory(pc + 1, &disp32, 4)) {
            return 0; // Error reading displacement
        }
        operand.mem.displacement = disp32;
        sib_length += 4;
    } else {
        // Normal base register
        operand.mem.base = base;
        operand.mem.displacement = 0; // Will be set by caller if mod != 0
    }
    
    // Handle index register
    if (index == 4) {
        // ESP cannot be index register, no index
        operand.mem.index = 0;
        operand.mem.scale = 1; // Scale is meaningless without index
    } else {
        operand.mem.index = index;
    }
    
    return sib_length;
}

} // namespace x86
} // namespace xenoarm