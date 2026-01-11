#ifndef MINIRISC_CONSTANTS_H
#define MINIRISC_CONSTANTS_H

// Opcodes Mini-RISC (table ISA)
#define OPCODE_LUI     0x01
#define OPCODE_AUIPC   0x02
#define OPCODE_JAL     0x03
#define OPCODE_JALR    0x04

#define OPCODE_BEQ     0x05
#define OPCODE_BNE     0x06
#define OPCODE_BLT     0x07
#define OPCODE_BGE     0x08
#define OPCODE_BLTU    0x09
#define OPCODE_BGEU    0x0A

#define OPCODE_LB      0x0B
#define OPCODE_LH      0x0C
#define OPCODE_LW      0x0D
#define OPCODE_LBU     0x0E
#define OPCODE_LHU     0x0F

#define OPCODE_SB      0x10
#define OPCODE_SH      0x11
#define OPCODE_SW      0x12

#define OPCODE_ADDI    0x13
#define OPCODE_SLTI    0x14
#define OPCODE_SLTIU   0x15
#define OPCODE_XORI    0x16
#define OPCODE_ORI     0x17
#define OPCODE_ANDI    0x18
#define OPCODE_SLLI    0x19
#define OPCODE_SRLI    0x1A
#define OPCODE_SRAI    0x1B

#define OPCODE_ADD     0x1C
#define OPCODE_SUB     0x1D
#define OPCODE_SLL     0x1E
#define OPCODE_SRL     0x1F
#define OPCODE_SRA     0x20
#define OPCODE_SLT     0x21
#define OPCODE_SLTU    0x22
#define OPCODE_XOR     0x23
#define OPCODE_OR      0x24
#define OPCODE_AND     0x25

#define OPCODE_ECALL   0x26
#define OPCODE_EBREAK  0x27

// (optionnel) M extension
#define OPCODE_MUL     0x38
#define OPCODE_MULH    0x39
#define OPCODE_MULHSU  0x3A
#define OPCODE_MULHU   0x3B
#define OPCODE_DIV     0x3C
#define OPCODE_DIVU    0x3D
#define OPCODE_REM     0x3E
#define OPCODE_REMU    0x3F

#endif
