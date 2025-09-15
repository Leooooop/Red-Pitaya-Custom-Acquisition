#include "rp.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdint.h>

#define PAGE_SIZE 4096

static int fd = -1;
static void *adr = NULL;

int rp_init(off_t base_addr) {
    fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd < 0) {
        perror("open /dev/mem");
        return -1;
    }

    adr = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, base_addr);
    if (adr == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return -1;
    }

    return 0;
}

void rp_write(int offset, int value) {
    if (adr) {
        *((volatile uint32_t *)((uint8_t *)adr + offset)) = value;
    }
}

int rp_read(int offset) {
    if (adr) {
        return *((volatile uint32_t *)((uint8_t *)adr + offset));
    }
    return -1; // error value
}

void rp_cleanup(void) {
    if (adr) {
        munmap(adr, PAGE_SIZE);
        adr = NULL;
    }
    if (fd >= 0) {
        close(fd);
        fd = -1;
    }
}
