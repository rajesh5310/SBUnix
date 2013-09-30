#include <sys/gdt.h>

/* adapted from Chris Stones, shovelos */

#define MAX_GDT 32

uint64_t gdt[MAX_GDT] = {
	0,                      /*** NULL descriptor ***/
	GDT_CS | P | DPL0 | L,  /*** kernel code segment descriptor ***/
	GDT_DS | P | W,         /*** kernel data segment descriptor ***/
};

struct gdtr_t {
	uint16_t size;
	uint64_t addr;
}__attribute__((packed));

static struct gdtr_t gdtr = {
	sizeof(gdt),
	(uint64_t)gdt,
};

void _x86_64_asm_lgdt(struct gdtr_t* gdtr, uint64_t cs_idx, uint64_t ds_idx);
void reload_gdt() {
	_x86_64_asm_lgdt(&gdtr, 8, 16);
}
