#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>

#include "rp.h" //include the driver
#include "python_linker.h" //including the declqrqtion of all the function used by python

int main()
{

off_t BASE = 0x43C00000; //adress for base register
  
rp_init(BASE);

printf("ID is: %#x\n", rp_read(0x00));

printf("value at 0x04 is (hexa): %#x\n", rp_read(0x04) - 8192);

printf("value at 0x04 is: %d\n", rp_read(0x04) - 8192);


rp_cleanup();
return 0;
}