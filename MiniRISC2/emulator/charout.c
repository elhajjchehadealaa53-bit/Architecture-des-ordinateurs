#include "charout.h"
#include <stdlib.h>
#include <stdio.h>

charout_t *charout_new(void) {
    return (charout_t*)calloc(1, sizeof(charout_t));
}

void charout_free(charout_t *co) {
    free(co);
}

static int in_range(uint32_t addr) {
    return (addr >= CHAROUT_BASE) && (addr < (CHAROUT_BASE + CHAROUT_SIZE));
}

int charout_read(charout_t *co, access_type_t type, uint32_t addr, uint32_t *data) {
    (void)co; (void)type;
    if (!in_range(addr)) return -1;
    *data = 0; // lecture renvoie 0
    return 0;
}

int charout_write(charout_t *co, access_type_t type, uint32_t addr, uint32_t data) {
    (void)co;
  //  printf("charout write 0x%08x @0x%08x\n", data, addr);
    if (!in_range(addr)) return -1;

    uint32_t reg = addr - CHAROUT_BASE;

    // reg0: char
    if (reg == 0x0) {
        unsigned char c = (unsigned char)(data & 0xFF);
        putchar(c);
        fflush(stdout);
        return 0;
    }

    // reg1: int32 decimal
    if (reg == 0x4) {
        if (type != ACCESS_WORD) return 0;
        printf("%d", (int32_t)data);
        fflush(stdout);
        return 0;
    }

    // reg2: uint32 hex
    if (reg == 0x8) {
        if (type != ACCESS_WORD) return 0;
        printf("0x%08x", (uint32_t)data);
        fflush(stdout);
        return 0;
    }

    return 0;
}
