#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>

#include "rp.h" //include the driver
#include "python_linker.h" //including the declqrqtion of all the function used by python

int loopback()
{

off_t BASE = 0x40000000; //adress for housekeeping
  
rp_init(BASE);
// 0x0c is the offset for loopback
printf("Loop back is: %d\n", rp_read(0x0c));

rp_write(0x0c,1);

printf("Loop back set to: %d\n", rp_read(0x0c));

rp_cleanup();
return 0;
}