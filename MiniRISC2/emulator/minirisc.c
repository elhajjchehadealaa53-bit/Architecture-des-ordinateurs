#include "minirisc.h"
#include "minirisc_constants.h"

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

/* ============================================================
   Mini-RISC (prof) : champs encodés différemment de RISC-V
   - opcode : bits  0..6
   - rd     : bits  7..11
   - rs1    : bits 12..16   (ATTENTION: pas 15..19)
   - rs2    : bits 17..21   (ATTENTION: pas 20..24)
   - imm I/S: bits 31..20
   - imm B  : bits 31..20 = imm[12:1], offset = signext(12) << 1
   - imm J  : bits 31..12 = imm[20:1], offset = signext(20) << 1
   - shamt  : bits 17..21 (même place que rs2)
   ============================================================ */

static inline uint32_t opcode(uint32_t ir) { return ir & 0x7Fu; }
static inline uint32_t rd(uint32_t ir)     { return (ir >> 7)  & 0x1Fu; }
static inline uint32_t rs1(uint32_t ir)    { return (ir >> 12) & 0x1Fu; }
static inline uint32_t rs2(uint32_t ir)    { return (ir >> 17) & 0x1Fu; }

static inline int32_t sign_extend(uint32_t v, int bits) {
    uint32_t m = 1u << (bits - 1);
    return (int32_t)((v ^ m) - m);
}

/* immediats Mini-RISC (prof) */
static inline int32_t imm_i(uint32_t ir) {
    // imm[11:0] en bits 31..20
    return sign_extend(ir >> 20, 12);
}

static inline int32_t imm_s(uint32_t ir) {
    // Mini-RISC: store utilise aussi bits 31..20 pour imm[11:0]
    return sign_extend(ir >> 20, 12);
}

static inline int32_t imm_b(uint32_t ir) {
    // Mini-RISC: bits 31..20 contiennent imm[12:1]
    int32_t off = sign_extend((ir >> 20) & 0xFFFu, 12);
    return off << 1; // adresse alignée par 2
}

static inline uint32_t imm_u(uint32_t ir) {
    // format U: imm[31..12]
    return ir & 0xFFFFF000u;
}

static inline int32_t imm_j(uint32_t ir) {
    // Mini-RISC: bits 31..12 contiennent imm[20:1]
    int32_t off = sign_extend((ir >> 12) & 0xFFFFFu, 20);
    return off << 1; // adresse alignée par 2
}

static inline uint32_t shamt(uint32_t ir) {
    // shamt[4:0] placé comme rs2 (bits 17..21)
    return (ir >> 17) & 0x1Fu;
}

minirisc_t *minirisc_new(uint32_t initial_PC, platform_t *platform) {
    minirisc_t *mr = (minirisc_t*)calloc(1, sizeof(minirisc_t));
    mr->PC = initial_PC;
    mr->platform = platform;
    mr->halt = 0;
    mr->regs[0] = 0;
    return mr;
}

void minirisc_free(minirisc_t *mr) {
    free(mr);
}

void minirisc_fetch(minirisc_t *mr) {
    uint32_t ins = 0;
    if (platform_read(mr->platform, ACCESS_WORD, mr->PC, &ins) != 0) {
        fprintf(stderr, "Erreur fetch: lecture invalide à PC=0x%08" PRIx32 "\n", mr->PC);
        mr->halt = 1;
        return;
    }
    mr->IR = ins;
    mr->next_PC = mr->PC + 4;
    //printf("@0x%08x: 0x%08x\n", mr->PC, mr->IR);
}

static void illegal(minirisc_t *mr, uint32_t op) {
    fprintf(stderr, "Erreur: opcode inconnu 0x%x à PC=0x%08" PRIx32 "\n", op, mr->PC);
    mr->halt = 1;
}

void minirisc_decode_and_execute(minirisc_t *mr) {
    uint32_t ir = mr->IR;
    uint32_t op = opcode(ir);

    uint32_t _rd  = rd(ir);
    uint32_t _rs1 = rs1(ir);
    uint32_t _rs2 = rs2(ir);

    uint32_t v1 = mr->regs[_rs1];
    uint32_t v2 = mr->regs[_rs2];

    switch (op) {
        /* ---------- U-type ---------- */
        case OPCODE_LUI:
            mr->regs[_rd] = imm_u(ir);
            break;

        case OPCODE_AUIPC:
            mr->regs[_rd] = mr->PC + imm_u(ir);
            break;

        /* ---------- Jumps ---------- */
        case OPCODE_JAL: {
            int32_t off = imm_j(ir);
            mr->regs[_rd] = mr->PC + 4;
            mr->next_PC = mr->PC + (uint32_t)off;
            break;
        }

        case OPCODE_JALR: {
            int32_t off = imm_i(ir);
            uint32_t target = (v1 + (uint32_t)off) & ~1u;
            mr->regs[_rd] = mr->PC + 4;
            mr->next_PC = target;
            break;
        }

        /* ---------- Branches ---------- */
        case OPCODE_BEQ:
            if (v1 == v2) mr->next_PC = mr->PC + (uint32_t)imm_b(ir);
            break;
        case OPCODE_BNE:
            if (v1 != v2) mr->next_PC = mr->PC + (uint32_t)imm_b(ir);
            break;
        case OPCODE_BLT:
            if ((int32_t)v1 < (int32_t)v2) mr->next_PC = mr->PC + (uint32_t)imm_b(ir);
            break;
        case OPCODE_BGE:
            if ((int32_t)v1 >= (int32_t)v2) mr->next_PC = mr->PC + (uint32_t)imm_b(ir);
            break;
        case OPCODE_BLTU:
            if (v1 < v2) mr->next_PC = mr->PC + (uint32_t)imm_b(ir);
            break;
        case OPCODE_BGEU:
            if (v1 >= v2) mr->next_PC = mr->PC + (uint32_t)imm_b(ir);
            break;

        /* ---------- Loads ---------- */
        case OPCODE_LB:
        case OPCODE_LBU:
        case OPCODE_LH:
        case OPCODE_LHU:
        case OPCODE_LW: {
            uint32_t addr = v1 + (uint32_t)imm_i(ir);
            uint32_t data = 0;

            access_type_t t = ACCESS_WORD;
            if (op == OPCODE_LB || op == OPCODE_LBU) t = ACCESS_BYTE;
            if (op == OPCODE_LH || op == OPCODE_LHU) t = ACCESS_HALF;
            if (op == OPCODE_LW) t = ACCESS_WORD;

            if (platform_read(mr->platform, t, addr, &data) != 0) {
                fprintf(stderr, "Erreur load à 0x%08" PRIx32 "\n", addr);
                mr->halt = 1;
                break;
            }

            if (op == OPCODE_LB)  mr->regs[_rd] = (uint32_t)(int32_t)(int8_t)(data & 0xFF);
            if (op == OPCODE_LBU) mr->regs[_rd] = data & 0xFF;
            if (op == OPCODE_LH)  mr->regs[_rd] = (uint32_t)(int32_t)(int16_t)(data & 0xFFFF);
            if (op == OPCODE_LHU) mr->regs[_rd] = data & 0xFFFF;
            if (op == OPCODE_LW)  mr->regs[_rd] = data;
            break;
        }

        /* ---------- Stores ---------- */
        case OPCODE_SB:
        case OPCODE_SH:
        case OPCODE_SW: {
            uint32_t addr = v1 + (uint32_t)imm_s(ir);

            access_type_t t = (op == OPCODE_SB) ? ACCESS_BYTE :
                              (op == OPCODE_SH) ? ACCESS_HALF :
                                                  ACCESS_WORD;

            if (platform_write(mr->platform, t, addr, mr->regs[_rd]) != 0) {
                fprintf(stderr, "Erreur store à 0x%08" PRIx32 "\n", addr);
                mr->halt = 1;
            }
            break;
        }

        /* ---------- ALU imm ---------- */
        case OPCODE_ADDI:
            mr->regs[_rd] = v1 + (uint32_t)imm_i(ir);
            break;
        case OPCODE_SLTI:
            mr->regs[_rd] = ((int32_t)v1 < (int32_t)imm_i(ir)) ? 1u : 0u;
            break;
        case OPCODE_SLTIU:
            mr->regs[_rd] = (v1 < (uint32_t)imm_i(ir)) ? 1u : 0u;
            break;
        case OPCODE_XORI:
            mr->regs[_rd] = v1 ^ (uint32_t)imm_i(ir);
            break;
        case OPCODE_ORI:
            mr->regs[_rd] = v1 | (uint32_t)imm_i(ir);
            break;
        case OPCODE_ANDI:
            mr->regs[_rd] = v1 & (uint32_t)imm_i(ir);
            break;
        case OPCODE_SLLI:
            mr->regs[_rd] = v1 << shamt(ir);
            break;
        case OPCODE_SRLI:
            mr->regs[_rd] = v1 >> shamt(ir);
            break;
        case OPCODE_SRAI:
            mr->regs[_rd] = (uint32_t)(((int32_t)v1) >> shamt(ir));
            break;

        /* ---------- ALU reg ---------- */
        case OPCODE_ADD:
            mr->regs[_rd] = v1 + v2;
            break;
        case OPCODE_SUB:
            mr->regs[_rd] = v1 - v2;
            break;
        case OPCODE_SLL:
            mr->regs[_rd] = v1 << (v2 & 0x1F);
            break;
        case OPCODE_SRL:
            mr->regs[_rd] = v1 >> (v2 & 0x1F);
            break;
        case OPCODE_SRA:
            mr->regs[_rd] = (uint32_t)(((int32_t)v1) >> (v2 & 0x1F));
            break;
        case OPCODE_SLT:
            mr->regs[_rd] = ((int32_t)v1 < (int32_t)v2) ? 1u : 0u;
            break;
        case OPCODE_SLTU:
            mr->regs[_rd] = (v1 < v2) ? 1u : 0u;
            break;
        case OPCODE_XOR:
            mr->regs[_rd] = v1 ^ v2;
            break;
        case OPCODE_OR:
            mr->regs[_rd] = v1 | v2;
            break;
        case OPCODE_AND:
            mr->regs[_rd] = v1 & v2;
            break;

        /* ---------- M extension (optionnel) ---------- */
        case OPCODE_MUL: {
            int64_t r = (int64_t)(int32_t)v1 * (int64_t)(int32_t)v2;
            mr->regs[_rd] = (uint32_t)r;
            break;
        }
        case OPCODE_MULH: {
            int64_t r = (int64_t)(int32_t)v1 * (int64_t)(int32_t)v2;
            mr->regs[_rd] = (uint32_t)((uint64_t)r >> 32);
            break;
        }
        case OPCODE_MULHU: {
            uint64_t r = (uint64_t)v1 * (uint64_t)v2;
            mr->regs[_rd] = (uint32_t)(r >> 32);
            break;
        }
        case OPCODE_MULHSU: {
            int64_t r = (int64_t)(int32_t)v1 * (int64_t)(uint32_t)v2;
            mr->regs[_rd] = (uint32_t)((uint64_t)r >> 32);
            break;
        }
        case OPCODE_DIV: {
            int32_t a = (int32_t)v1, b = (int32_t)v2;
            mr->regs[_rd] = (b == 0) ? 0xFFFFFFFFu : (uint32_t)(a / b);
            break;
        }
        case OPCODE_DIVU:
            mr->regs[_rd] = (v2 == 0) ? 0xFFFFFFFFu : (v1 / v2);
            break;
        case OPCODE_REM: {
            int32_t a = (int32_t)v1, b = (int32_t)v2;
            mr->regs[_rd] = (b == 0) ? v1 : (uint32_t)(a % b);
            break;
        }
        case OPCODE_REMU:
            mr->regs[_rd] = (v2 == 0) ? v1 : (v1 % v2);
            break;

        /* ---------- System ---------- */
        case OPCODE_EBREAK:
            mr->halt = 1;
            break;

        default:
            illegal(mr, op);
            break;
    }

    // x0 toujours = 0
    mr->regs[0] = 0;

    // avancer PC
    mr->PC = mr->next_PC;
}

void minirisc_run(minirisc_t *mr) {
    while (!mr->halt) {
        minirisc_fetch(mr);
        if (mr->halt) break;
        minirisc_decode_and_execute(mr);
    }
}