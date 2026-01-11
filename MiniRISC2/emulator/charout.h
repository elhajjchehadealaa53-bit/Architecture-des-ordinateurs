#ifndef CHAROUT_H
#define CHAROUT_H

#include <stdint.h>
#include "access_type.h"

#define CHAROUT_BASE 0x10000000u
#define CHAROUT_SIZE 0x0000000Cu

typedef struct {
    int dummy;
} charout_t;

charout_t *charout_new(void);
void charout_free(charout_t *co);

int charout_read(charout_t *co, access_type_t type, uint32_t addr, uint32_t *data);
int charout_write(charout_t *co, access_type_t type, uint32_t addr, uint32_t data);

#endif
