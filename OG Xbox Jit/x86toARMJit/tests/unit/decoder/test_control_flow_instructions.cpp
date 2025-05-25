#include <gtest/gtest.h>
#include <vector>
#include <cstdint>
#include "decoder/decoder.h"
#include "runtime/memory_interface.h"

using namespace xenoarm::x86;

// Mock memory for testing decoder
class MockMemory : public xenoarm::IMemoryInterface {
public:
    MockMemory(std::vector<uint8_t> data) : data_(std::move(data)) {}
    
    bool readMemory(uint32_t addr, void* buffer, size_t size) override {
        if (addr + size > data_.size()) {
            return false;
        }
        std::memcpy(buffer, data_.data() + addr, size);
        return true;
    }
    
    bool writeMemory(uint32_t addr, const void* buffer, size_t size) override {
        return false;
    }
    
    void* getHostPointer(uint32_t addr) override {
        return nullptr;
    }
    
    void registerWriteCallback(uint32_t start_addr, size_t size, 
                             std::function<void(uint32_t)> callback) override {
    }
    
    bool isExecutable(uint32_t addr) override {
        return true;
    }
    
private:
    std::vector<uint8_t> data_;
};

class ControlFlowTest : public ::testing::Test {
protected:
    void SetUp() override {
        decoder = std::make_unique<Decoder>();
    }
    
    std::unique_ptr<IDecoder> decoder;
};

// Test short JMP (EB)
TEST_F(ControlFlowTest, DecodeJmpShort) {
    // JMP +16 (0xEB 0x10)
    MockMemory memory({0xEB, 0x10});
    
    Instruction inst;
    size_t bytes = decoder->decodeInstruction(&memory, 0, inst);
    
    EXPECT_EQ(bytes, 2);
    EXPECT_EQ(inst.opcode, Instruction::Opcode::JMP);
    EXPECT_EQ(inst.dst.type, Instruction::Operand::Type::IMMEDIATE);
    EXPECT_EQ(inst.dst.imm, 0x10);
}

// Test near JMP (E9)
TEST_F(ControlFlowTest, DecodeJmpNear) {
    // JMP +256 (0xE9 0x00 0x01 0x00 0x00)
    MockMemory memory({0xE9, 0x00, 0x01, 0x00, 0x00});
    
    Instruction inst;
    size_t bytes = decoder->decodeInstruction(&memory, 0, inst);
    
    EXPECT_EQ(bytes, 5);
    EXPECT_EQ(inst.opcode, Instruction::Opcode::JMP);
    EXPECT_EQ(inst.dst.type, Instruction::Operand::Type::IMMEDIATE);
    EXPECT_EQ(inst.dst.imm, 0x00000100);
}

// Test JMP with ModR/M (FF /4)
TEST_F(ControlFlowTest, DecodeJmpIndirect) {
    // JMP EAX (0xFF 0xE0)
    // ModR/M = 0xE0 = 11100000b
    // Mod = 11 (register), Reg = 100 (/4), R/M = 000 (EAX)
    MockMemory memory({0xFF, 0xE0});
    
    Instruction inst;
    size_t bytes = decoder->decodeInstruction(&memory, 0, inst);
    
    EXPECT_EQ(bytes, 2);
    EXPECT_EQ(inst.opcode, Instruction::Opcode::JMP);
    EXPECT_EQ(inst.dst.type, Instruction::Operand::Type::REGISTER);
    EXPECT_EQ(inst.dst.reg, 0); // EAX
}

// Test JE (Jump if Equal) short
TEST_F(ControlFlowTest, DecodeJccShort) {
    // JE +5 (0x74 0x05)
    MockMemory memory({0x74, 0x05});
    
    Instruction inst;
    size_t bytes = decoder->decodeInstruction(&memory, 0, inst);
    
    EXPECT_EQ(bytes, 2);
    EXPECT_EQ(inst.opcode, Instruction::Opcode::JCC);
    EXPECT_EQ(inst.dst.type, Instruction::Operand::Type::IMMEDIATE);
    EXPECT_EQ(inst.dst.imm, 0x05);
    // TODO: Need a way to store condition code (E = Equal)
}

// Test JNE (Jump if Not Equal) short with negative offset
TEST_F(ControlFlowTest, DecodeJneShortNegative) {
    // JNE -2 (0x75 0xFE)
    MockMemory memory({0x75, 0xFE});
    
    Instruction inst;
    size_t bytes = decoder->decodeInstruction(&memory, 0, inst);
    
    EXPECT_EQ(bytes, 2);
    EXPECT_EQ(inst.opcode, Instruction::Opcode::JCC);
    EXPECT_EQ(inst.dst.type, Instruction::Operand::Type::IMMEDIATE);
    // Sign-extended -2
    EXPECT_EQ(static_cast<int32_t>(inst.dst.imm), -2);
}

// Test JGE (Jump if Greater or Equal) near
TEST_F(ControlFlowTest, DecodeJccNear) {
    // JGE +4096 (0x0F 0x8D 0x00 0x10 0x00 0x00)
    MockMemory memory({0x0F, 0x8D, 0x00, 0x10, 0x00, 0x00});
    
    Instruction inst;
    size_t bytes = decoder->decodeInstruction(&memory, 0, inst);
    
    EXPECT_EQ(bytes, 6);
    EXPECT_EQ(inst.opcode, Instruction::Opcode::JCC);
    EXPECT_EQ(inst.dst.type, Instruction::Operand::Type::IMMEDIATE);
    EXPECT_EQ(inst.dst.imm, 0x00001000);
}

// Test CALL near
TEST_F(ControlFlowTest, DecodeCallNear) {
    // CALL +512 (0xE8 0x00 0x02 0x00 0x00)
    MockMemory memory({0xE8, 0x00, 0x02, 0x00, 0x00});
    
    Instruction inst;
    size_t bytes = decoder->decodeInstruction(&memory, 0, inst);
    
    EXPECT_EQ(bytes, 5);
    EXPECT_EQ(inst.opcode, Instruction::Opcode::CALL);
    EXPECT_EQ(inst.dst.type, Instruction::Operand::Type::IMMEDIATE);
    EXPECT_EQ(inst.dst.imm, 0x00000200);
}

// Test CALL indirect register
TEST_F(ControlFlowTest, DecodeCallIndirectReg) {
    // CALL EDX (0xFF 0xD2)
    // ModR/M = 0xD2 = 11010010b
    // Mod = 11 (register), Reg = 010 (/2), R/M = 010 (EDX)
    MockMemory memory({0xFF, 0xD2});
    
    Instruction inst;
    size_t bytes = decoder->decodeInstruction(&memory, 0, inst);
    
    EXPECT_EQ(bytes, 2);
    EXPECT_EQ(inst.opcode, Instruction::Opcode::CALL);
    EXPECT_EQ(inst.dst.type, Instruction::Operand::Type::REGISTER);
    EXPECT_EQ(inst.dst.reg, 2); // EDX
}

// Test CALL memory indirect
TEST_F(ControlFlowTest, DecodeCallIndirectMem) {
    // CALL [0x1234] (0xFF 0x15 0x34 0x12 0x00 0x00)
    // ModR/M = 0x15 = 00010101b
    // Mod = 00 (memory, disp32), Reg = 010 (/2), R/M = 101 (disp32)
    MockMemory memory({0xFF, 0x15, 0x34, 0x12, 0x00, 0x00});
    
    Instruction inst;
    size_t bytes = decoder->decodeInstruction(&memory, 0, inst);
    
    EXPECT_EQ(bytes, 6);
    EXPECT_EQ(inst.opcode, Instruction::Opcode::CALL);
    EXPECT_EQ(inst.dst.type, Instruction::Operand::Type::MEMORY);
    EXPECT_EQ(inst.dst.mem.displacement, 0x00001234);
}

// Test RET
TEST_F(ControlFlowTest, DecodeRet) {
    // RET (0xC3)
    MockMemory memory({0xC3});
    
    Instruction inst;
    size_t bytes = decoder->decodeInstruction(&memory, 0, inst);
    
    EXPECT_EQ(bytes, 1);
    EXPECT_EQ(inst.opcode, Instruction::Opcode::RET);
}

// Test RET with immediate
TEST_F(ControlFlowTest, DecodeRetImm) {
    // RET 8 (0xC2 0x08 0x00)
    MockMemory memory({0xC2, 0x08, 0x00});
    
    Instruction inst;
    size_t bytes = decoder->decodeInstruction(&memory, 0, inst);
    
    EXPECT_EQ(bytes, 3);
    EXPECT_EQ(inst.opcode, Instruction::Opcode::RET);
    EXPECT_EQ(inst.src.type, Instruction::Operand::Type::IMMEDIATE);
    EXPECT_EQ(inst.src.imm, 0x0008);
}

// Test LOOP
TEST_F(ControlFlowTest, DecodeLoop) {
    // LOOP -4 (0xE2 0xFC)
    MockMemory memory({0xE2, 0xFC});
    
    Instruction inst;
    size_t bytes = decoder->decodeInstruction(&memory, 0, inst);
    
    EXPECT_EQ(bytes, 2);
    EXPECT_EQ(inst.opcode, Instruction::Opcode::LOOP);
    EXPECT_EQ(inst.dst.type, Instruction::Operand::Type::IMMEDIATE);
    // Sign-extended -4
    EXPECT_EQ(static_cast<int32_t>(inst.dst.imm), -4);
}

// Test LOOPE/LOOPZ
TEST_F(ControlFlowTest, DecodeLoopE) {
    // LOOPE +16 (0xE1 0x10)
    MockMemory memory({0xE1, 0x10});
    
    Instruction inst;
    size_t bytes = decoder->decodeInstruction(&memory, 0, inst);
    
    EXPECT_EQ(bytes, 2);
    EXPECT_EQ(inst.opcode, Instruction::Opcode::LOOP);
    EXPECT_EQ(inst.dst.type, Instruction::Operand::Type::IMMEDIATE);
    EXPECT_EQ(inst.dst.imm, 0x10);
    // TODO: Need a way to distinguish LOOPE from LOOP
}

// Test LOOPNE/LOOPNZ
TEST_F(ControlFlowTest, DecodeLoopNE) {
    // LOOPNE +32 (0xE0 0x20)
    MockMemory memory({0xE0, 0x20});
    
    Instruction inst;
    size_t bytes = decoder->decodeInstruction(&memory, 0, inst);
    
    EXPECT_EQ(bytes, 2);
    EXPECT_EQ(inst.opcode, Instruction::Opcode::LOOP);
    EXPECT_EQ(inst.dst.type, Instruction::Operand::Type::IMMEDIATE);
    EXPECT_EQ(inst.dst.imm, 0x20);
    // TODO: Need a way to distinguish LOOPNE from LOOP
}