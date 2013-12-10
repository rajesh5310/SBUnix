#include <defs.h>

void reload_idt();
void load_irq();
struct idt_entry_struct
{
    uint16_t offset_low;
    uint16_t selector;
    unsigned ist : 3;
    unsigned reserved0 : 5;
    unsigned type : 4;
    unsigned zero : 1;
    unsigned dpl : 2;
    unsigned p : 1;
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t reserved1;
} __attribute__((packed));



typedef struct idt_entry_struct idt_entry_t;

struct idt_ptr_struct
{
    uint16_t limit;
    uint64_t base;
} __attribute__ ((packed));

typedef struct idt_ptr_struct idt_ptr_t;
extern void _x86_64_asm_igdt(struct idt_ptr_struct* idtr);

