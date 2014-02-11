#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "m5op.h"

/*
 * Wow they really managed to make this extra complicated. Basically the m5op (x86 only)
 * now needs to operate by copying in data to an mmap'd region which a generic m5op 
 * pulls data from. For standalone code (outside m5.c), we need to copy this function.
 *
 * Hopefully this won't change, or they decide to move this to a separate header/source
 * file pair.
 */
void *m5_mem = NULL;

static void
map_m5_mem()
{
#ifdef M5OP_ADDR
    int fd;

    fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd == -1) {
        perror("Can't open /dev/mem");
        exit(1);
    }

    m5_mem = mmap(NULL, 0x10000, PROT_READ | PROT_WRITE, MAP_SHARED, fd, M5OP_ADDR);
    if (!m5_mem) {
        perror("Can't mmap /dev/mem");
        exit(1);
    }
#endif
}


int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Usage: %s stall_cycles\n", argv[0]);
        return 1;
    }

    map_m5_mem();

    uint64_t stall_cycles = atoi(argv[1]);
    uint64_t stall_count = 0;

    while (stall_count < stall_cycles) {
        stall_count = m5_getcpuinst(0);
        //printf("stalled for %d insts so far.\n", stall_count);
        printf("waiting...\r");
    }

    printf("Stalled for %d cycles.\n", stall_count);

    return 0;
}

