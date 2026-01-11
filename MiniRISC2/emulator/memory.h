#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
#include "access_type.h"

#define MEM_BASE 0x80000000u
#define MEM_SIZE (32u * 1024u * 1024u)

typedef struct {
    uint8_t *bytes;
} memory_t;

memory_t *memory_new(void);
void memory_free(memory_t *m);

int memory_read(memory_t *m, access_type_t type, uint32_t addr, uint32_t *data);
int memory_write(memory_t *m, access_type_t type, uint32_t addr, uint32_t data);

#endif
