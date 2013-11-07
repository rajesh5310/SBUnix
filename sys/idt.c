/* @name : idt.c
 * @author : rgolani, abmishra, skandalamsri
 * @last updated date : 25th September, 2013
 */

#include <sys/idt.h>
#include <ports.h>
#include <print.h>
#include <sys/isr.h>
static void init_idt();

/*
* This method sets the entry for the secified interrupt and maps it to passed method
*/
static void idt_set_gate(int,uint16_t, unsigned , unsigned , unsigned , uint64_t);

idt_entry_t idt_entries[256];
idt_ptr_t idt_ptr;

void reload_idt()
{
    init_idt();
};

// Write len copies of val into dest.


extern void isr0(); //DUMMY_INTERRUPT(0)
extern void isr32();
extern void isr33();

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
      __asm__("sti");
}

static void init_idt()
{
    memset(&idt_entries, 0, sizeof(idt_entry_t ) * 256);
    idt_ptr.limit = sizeof(idt_entry_t) * 256 - 1;
    idt_ptr.base = (uint64_t)&idt_entries;

    idt_set_gate(0,8, 0, 0x0e, 0, ((uint64_t)&isr0));
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
