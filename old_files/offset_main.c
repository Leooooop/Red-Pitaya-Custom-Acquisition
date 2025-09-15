#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>  // for strcmp

#include "rp.h" //include the driver

int main(int argc, char *argv[])
{
    if (argc < 3) {
        fprintf(stderr, "ERROR => You should write: %s <value> <+|->\n", argv[0]);
        return 1;
    }
    off_t BASE = 0x40300000; // PID base address


    char plus_minus = argv[2][0];  // Take the first character of the string argument
    int offset_adr;

    if (plus_minus == '+') {
        offset_adr = 0x60;
    } else if (plus_minus == '-') {
        offset_adr = 0x70;
    } else {
        fprintf(stderr, "Wrong operand: use '+' or '-'\n");
        return 1;
    }

    rp_init(BASE);

    printf("Initial val at %X was: %d\n", offset_adr, rp_read(offset_adr));

    int value = strtol(argv[1], NULL, 0);  // Handles both hex and decimal input

    rp_write(offset_adr,value);

    printf("Value 0x%X written to offset 0x%X\n", value, offset_adr);

    rp_cleanup();
    return 0;
}
