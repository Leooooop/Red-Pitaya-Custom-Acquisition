#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>

#include "rp.h" //include the driver
#include "python_linker.h" //including the declqrqtion of all the function used by python

int multiply(int value)
{
  off_t BASE = 0x40300000; // PID base address;

  rp_init(BASE);

  printf("Initial val was: %d\n", rp_read(0x54));

  /* write to "a" register */
  rp_write(0x54,value);

  printf("It is now set to: %d\n", rp_read(0x54));
  printf("input value: %d (0x%X)\n", value, value);

  rp_cleanup();
  return 0;
}