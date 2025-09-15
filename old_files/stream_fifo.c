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

    printf("Streaming ADC values (Ctrl+C to stop)\n");

    while (1) {
        // Lire la valeur ADC dans le registre 0x04
        int32_t val = rp_read(0x04) - 8192; // centré à 0 pour 14 bits

        // Afficher la valeur
        printf("%d\n", val);
        fflush(stdout); // envoi immédiat à la console

        usleep(10000); // lecture à 100 Hz (~10 ms entre échantillons)
    }

    rp_cleanup();
    return 0;
}
