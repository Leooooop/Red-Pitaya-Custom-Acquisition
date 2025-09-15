#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>

#include "rp.h"
#include "python_linker.h"

int main()
{
    off_t BASE = 0x40000000; // address for base register

    rp_init(BASE);

    FILE *csv = fopen("data.csv", "w");
    if (csv == NULL) {
        perror("fopen");
        rp_cleanup();
        return 1;
    }

    fprintf(csv, "index,value\n"); // En-tête CSV

    for (int i = 0; i < 4000; i++) {
        rp_write(0x08, 1);   
        rp_write(0x08, 0); 
        int32_t val = rp_read(0x04);

        fprintf(csv, "%d,%d\n", i, val);
    }

    fclose(csv);
    rp_cleanup();
    return 0;
}
