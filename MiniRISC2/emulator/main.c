#include <stdio.h>
#include "platform.h"
#include "minirisc.h"
#include "memory.h"
#include "access_type.h"
#include "memory.h"
int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: %s program.bin\n", argv[0]);
        return 1;
    }

    platform_t *plt = platform_new();
    minirisc_t *cpu = minirisc_new(MEM_BASE, plt);

    platform_load_program(plt, argv[1]);
    uint32_t first = 0;
    platform_read(plt, ACCESS_WORD, MEM_BASE, &first);
  
    printf("Premier mot @0x%08x = 0x%08x\n", MEM_BASE, first);
    minirisc_run(cpu);

    minirisc_free(cpu);
    platform_free(plt);
    return 0;
}
