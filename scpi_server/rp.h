#ifndef RP_H
#define RP_H

#include <stdint.h>
#include <sys/types.h>

//initialize the connection mmap
int rp_init(off_t base_addr);
//write data to a certain adress connected with an offset
void rp_write(int offset, int value);
//read data from that adress
int rp_read(int offset);
//mmunmap close connection
void rp_cleanup(void);

#endif
