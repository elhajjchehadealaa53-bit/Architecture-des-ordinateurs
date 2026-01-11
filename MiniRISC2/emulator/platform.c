#include "platform.h"
#include <stdio.h>
#include <stdlib.h>

platform_t *platform_new(void) {
    platform_t *plt = (platform_t*)calloc(1, sizeof(platform_t));
    plt->mem = memory_new();
    plt->charout = charout_new();
    return plt;
}

void platform_free(platform_t *plt) {
    if (!plt) return;
    charout_free(plt->charout);
    memory_free(plt->mem);
    free(plt);
}

int platform_read(platform_t *plt, access_type_t type, uint32_t addr, uint32_t *data) {
    // CharOut ?
    if (addr >= CHAROUT_BASE && addr < (CHAROUT_BASE + CHAROUT_SIZE)) {
        return charout_read(plt->charout, type, addr, data);
    }
    // RAM
    return memory_read(plt->mem, type, addr, data);
}

int platform_write(platform_t *plt, access_type_t type, uint32_t addr, uint32_t data) {
 //   printf("platform write 0x%08x @0x%08x\n", data, addr);
    // CharOut ?
    if (addr >= CHAROUT_BASE && addr < (CHAROUT_BASE + CHAROUT_SIZE)) {
        return charout_write(plt->charout, type, addr, data);
    }
    // RAM
    return memory_write(plt->mem, type, addr, data);
}

void platform_load_program(platform_t *plt, const char *file_name) {
    FILE *f = fopen(file_name, "rb");
    if (!f) {
        perror("fopen");
        return;
    }
    // charge à partir de MEM_BASE (offset 0 dans memory.bytes)
    fread(plt->mem->bytes, 1, MEM_SIZE, f);
    fclose(f);
}
