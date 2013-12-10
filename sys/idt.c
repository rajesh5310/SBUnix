#include <sys/idt.h>
#include <sys/ports.h>
#include <sys/print.h>
#include <sys/isr.h>
static void init_idt();
static void idt_set_gate(int,uint16_t, unsigned , unsigned , unsigned , uint64_t);

idt_entry_t idt_entries[256];
idt_ptr_t idt_ptr;

void reload_idt()
{
    init_idt();
};

// Write len copies of val into dest.


extern void isr0(); //DUMMY_INTERRUPT(0)
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();
extern void isr32();
extern void isr33();
extern void isr128();
extern void isr43();
void load_irq()
{
      outb(0x20, 0x11);
      outb(0xA0, 0x11);
      outb(0x21, 0x20);
      outb(0xA1, 0x28);
      outb(0x21, 0x04);
      outb(0xA1, 0x02);
      outb(0x21, 0x01);
      outb(0xA1, 0x01);
      outb(0x21, 0x0);
      outb(0xA1, 0x0);

      idt_set_gate(32,8, 0, 0x8E, 0, ((uint64_t)isr32));
      idt_set_gate(33,8, 0, 0x8E, 0, ((uint64_t)isr33));
      idt_set_gate(43,8, 0, 0x8E, 0, ((uint64_t)isr43));

//      __asm__("sti");
}

static void init_idt()
{
    memset(&idt_entries, 0, sizeof(idt_entry_t ) * 256);
    idt_ptr.limit = sizeof(idt_entry_t) * 256 - 1;
    idt_ptr.base = (uint64_t)&idt_entries;

    idt_set_gate(0,8, 0, 0x0e, 0, ((uint64_t)&isr0));
    idt_set_gate(1,8, 0, 0x0e, 0, ((uint64_t)&isr1));
    idt_set_gate(2,8, 0, 0x0e, 0, ((uint64_t)&isr2));
    idt_set_gate(3,8, 0, 0x0e, 0, ((uint64_t)&isr3));
    idt_set_gate(4,8, 0, 0x0e, 0, ((uint64_t)&isr4));
    idt_set_gate(5,8, 0, 0x0e, 0, ((uint64_t)&isr5));
    idt_set_gate(6,8, 0, 0x0e, 0, ((uint64_t)&isr6));
    idt_set_gate(7,8, 0, 0x0e, 0, ((uint64_t)&isr7));
    idt_set_gate(8,8, 0, 0x0e, 0, ((uint64_t)&isr8));
    idt_set_gate(9,8, 0, 0x0e, 0, ((uint64_t)&isr9));
    idt_set_gate(10,8, 0, 0x0e, 0, ((uint64_t)&isr10));
    idt_set_gate(11,8, 0, 0x0e, 0, ((uint64_t)&isr11));
    idt_set_gate(12,8, 0, 0x0e, 0, ((uint64_t)&isr12));
    idt_set_gate(13,8, 0, 0x0e, 0, ((uint64_t)&isr13));
    idt_set_gate(14,8, 0, 0x0e, 0, ((uint64_t)&isr14));
    idt_set_gate(15,8, 0, 0x0e, 0, ((uint64_t)&isr15));
    idt_set_gate(16,8, 0, 0x0e, 0, ((uint64_t)&isr16));
    idt_set_gate(17,8, 0, 0x0e, 0, ((uint64_t)&isr17));
    idt_set_gate(18,8, 0, 0x0e, 0, ((uint64_t)&isr18));
    idt_set_gate(19,8, 0, 0x0e, 0, ((uint64_t)&isr19));
    idt_set_gate(20,8, 0, 0x0e, 0, ((uint64_t)&isr20));
    idt_set_gate(21,8, 0, 0x0e, 0, ((uint64_t)&isr21));
    idt_set_gate(22,8, 0, 0x0e, 0, ((uint64_t)&isr22));
    idt_set_gate(23,8, 0, 0x0e, 0, ((uint64_t)&isr23));
    idt_set_gate(24,8, 0, 0x0e, 0, ((uint64_t)&isr24));
    idt_set_gate(25,8, 0, 0x0e, 0, ((uint64_t)&isr25));
    idt_set_gate(26,8, 0, 0x0e, 0, ((uint64_t)&isr26));
    idt_set_gate(27,8, 0, 0x0e, 0, ((uint64_t)&isr27));
    idt_set_gate(28,8, 0, 0x0e, 0, ((uint64_t)&isr28));
    idt_set_gate(29,8, 0, 0x0e, 0, ((uint64_t)&isr29));
    idt_set_gate(30,8, 0, 0x0e, 0, ((uint64_t)&isr30));
    idt_set_gate(31,8, 0, 0x0e, 0, ((uint64_t)&isr31));
    idt_set_gate(128,8, 0, 0x0e, 3, ((uint64_t)&isr128));
    _x86_64_asm_igdt(&idt_ptr);
};

static void idt_set_gate(int num,uint16_t selector, unsigned ist, unsigned type, unsigned dpl, uint64_t offset)
{
    idt_entries[num].offset_low = (offset) & 0xFFFF;
    idt_entries[num].selector = selector;
    idt_entries[num].ist = ist;
    idt_entries[num].reserved0 = 0;
    idt_entries[num].type = type;
    idt_entries[num].zero = 0;
    idt_entries[num].dpl = dpl;
    idt_entries[num].p = 1;
    idt_entries[num].offset_mid = (offset >> 16) & 0xFFFF;
    idt_entries[num].offset_high = (offset >> 32) & 0xFFFFFFFF;
};
