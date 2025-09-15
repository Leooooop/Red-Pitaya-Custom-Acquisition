#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>

#include "rp.h" //include the driver

int main(int argc, char *argv[])
{

  if (argc < 2) {
        fprintf(stderr, "ERROR => You should write: %s <value>\n", argv[0]);
        return 1;
    }
  off_t BASE = 0x40300000; // PID base address;

  rp_init(BASE);

  printf("Initial val was: %d\n", rp_read(0x54));

  int value = strtol(argv[1], NULL, 0);  // base 0 auto-detects hex/dec
  /* write to "a" register */
  rp_write(0x54,value);

  printf("It is now set to: %d\n", rp_read(0x54));
  printf("input value: %d (0x%X)\n", value, value);

  rp_cleanup();
  return 0;
}