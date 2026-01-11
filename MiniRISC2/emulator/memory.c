#include "memory.h"
#include <stdlib.h>

static int check_align(access_type_t t, uint32_t addr) {
    if (t == ACCESS_HALF) return (addr % 2u) == 0u;
    if (t == ACCESS_WORD) return (addr % 4u) == 0u;
    return 1;
}

static int in_range(uint32_t addr) {
    return (addr >= MEM_BASE) && (addr < (MEM_BASE + MEM_SIZE));
}

memory_t *memory_new(void) {
    memory_t *m = (memory_t*)calloc(1, sizeof(memory_t));
    m->bytes = (uint8_t*)calloc(1, MEM_SIZE);
    return m;
}

void memory_free(memory_t *m) {
    if (!m) return;
    free(m->bytes);
    free(m);
}

int memory_read(memory_t *m, access_type_t type, uint32_t addr, uint32_t *data) {
    if (!in_range(addr) || !check_align(type, addr)) return -1;
    uint32_t off = addr - MEM_BASE;

    if (type == ACCESS_BYTE) {
        *data = m->bytes[off];
        return 0;
    }
    if (type == ACCESS_HALF) {
        *data = (uint32_t)m->bytes[off]
              | ((uint32_t)m->bytes[off+1] << 8);
        return 0;
    }
    // ACCESS_WORD
    *data = (uint32_t)m->bytes[off]
          | ((uint32_t)m->bytes[off+1] << 8)
          | ((uint32_t)m->bytes[off+2] << 16)
          | ((uint32_t)m->bytes[off+3] << 24);
    return 0;
}

int memory_write(memory_t *m, access_type_t type, uint32_t addr, uint32_t data) {
    if (!in_range(addr) || !check_align(type, addr)) return -1;
    uint32_t off = addr - MEM_BASE;

    if (type == ACCESS_BYTE) {
        m->bytes[off] = (uint8_t)(data & 0xFF);
        return 0;
    }
    if (type == ACCESS_HALF) {
        m->bytes[off]   = (uint8_t)(data & 0xFF);
        m->bytes[off+1] = (uint8_t)((data >> 8) & 0xFF);
        return 0;
    }
    // ACCESS_WORD
    m->bytes[off]   = (uint8_t)(data & 0xFF);
    m->bytes[off+1] = (uint8_t)((data >> 8) & 0xFF);
    m->bytes[off+2] = (uint8_t)((data >> 16) & 0xFF);
    m->bytes[off+3] = (uint8_t)((data >> 24) & 0xFF);
    return 0;
}
