#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdint.h>
#include "access_type.h"
#include "memory.h"
#include "charout.h"

typedef struct {
    memory_t  *mem;
    charout_t *charout;
} platform_t;

platform_t *platform_new(void);
void platform_free(platform_t *plt);

int platform_read(platform_t *plt, access_type_t type, uint32_t addr, uint32_t *data);
int platform_write(platform_t *plt, access_type_t type, uint32_t addr, uint32_t data);

void platform_load_program(platform_t *plt, const char *file_name);

#endif
