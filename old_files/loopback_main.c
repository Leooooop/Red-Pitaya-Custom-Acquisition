#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>

#include "rp.h" //include the driver

#define BASE_ADDR 0x40000000

int main(int argc, char **argv)
{

off_t BASE = BASE_ADDR; //adress for housekeeping
  
rp_init(BASE);

printf("Loop back is: %d\n", rp_read(0x0c));

rp_write(0x0c,1);

printf("Loop back set to: %d\n", rp_read(0x0c));

rp_cleanup();
return 0;
}