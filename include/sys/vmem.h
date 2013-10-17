#include<defs.h>
#include<define.h>
#include<ports.h>
#include<print.h>
#include<sys/pmap.h>
#ifndef _VMEM_H
#define _VMEM_H
struct smap_t {
uint64_t base, length;
uint32_t type;
}__attribute__((packed)) *smap;

void init_phy_mem(uint32_t*, void* , char*);
#endif
