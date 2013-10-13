#include<defs.h>
#include<define.h>
#include<ports.h>
#ifndef _VMEM_H
#define _VMEM_H
struct smap_t {
uint64_t base, length;
uint32_t type;
}__attribute__((packed)) *smap;


void init_phy_mem(uint32_t*, void* , void*);
void set_mem_bit(int position);
int get_mem_bit(int position);


struct pg_dir
{
    uint64_t pg_dir_ptr[512];
} *pg_dir;

struct pg_tbl1
{
    uint64_t pg_tbl1_ptr[512];
};

struct pg_tbl2
{
    uint64_t pg_tbl2_ptr[512];
};

struct pg_tbl3
{
    uint64_t pg_tbl3_ptr[512];
};

struct pg_tbl1 *pg_tbl1_1;
struct pg_tbl2 *pg_tbl2_1_5;
struct pg_tbl3 *pg_tbl3_1_5_10;



uint64_t get_pg_dir(uint64_t index);
uint64_t get_pg_tbl1(uint64_t index);
uint64_t get_pg_tbl2(uint64_t index);
uint64_t get_pg_tbl3(uint64_t index);

uint64_t set_pg_dir(uint64_t index, uint64_t value);
uint64_t set_pg_tbl1(uint64_t index, uint64_t value);
uint64_t set_pg_tbl2(uint64_t index, uint64_t value);
uint64_t set_pg_tbl3(uint64_t indx, uint64_t value);


#endif
