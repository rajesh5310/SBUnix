#include<sys/vmem.h>
#include<print.h>
#include<defs.h>
void setup_page_tables(char* physfree);
char phy_mem_map[NUMBER_OF_PAGES/8]; //1 bit for each 8 bits ; 1 character takes 8 bits and can hence store 8 pages.24b000_____
uint64_t kmalloc(uint64_t sz, char *physfree, int align);
void switch_page_directory(page_dir *dir);
extern void load_cr3(page_dir *dir);
//Intialization of virtual memory
void init_phy_mem(uint32_t* modulep, void* physbase, char* physfree)
{
    uint64_t total_phy_mem = 0, avail_phy_mem = 0, length=0;
    uint64_t i = 0;

    //List available memory locations
    while(modulep[0] != 0x9001) modulep += modulep[1]+2;

    for(smap = (struct smap_t*)(modulep+2); smap < (struct smap_t*)((char*)modulep+modulep[1]+2*4); ++smap)
    {
        if (smap->type == 1 /* memory */ && smap->length != 0)
        {
            print("\nAvailable Physical Memory [%x-%x]", smap->base, smap->base + smap->length);
            avail_phy_mem += smap->length;
            length += smap->length;
        }
        else
            print("\nRESERVED mem [%x-%x] SIZE: %u ", smap->base, smap->base + smap->length, smap->length);
        total_phy_mem += smap->length;
    }

    print("\nModulep [0] = %p", modulep[0]);

    memset(phy_mem_map, 0x00, sizeof(phy_mem_map)); //Set All bits Available

    // SET BITS UNAVAILABLE FOR the pages corresponding from physbaffffffff80205018se to physfree
    for (i = (uint64_t) physbase / PAGE_SIZE; i < ((uint64_t)physfree /PAGE_SIZE); i++)
        set_mem_bit(i);

    setup_page_tables(physfree);

}
//Function for setting up page tables
void setup_page_tables(char* physfree)
{

    page_dir *pg_dir = (page_dir *)kmalloc(sizeof(page_dir), physfree, 1);
    print("\nAddress : %x", pg_dir);

    pg_tbl1 *pg_tbl1_i = (pg_tbl1 *)kmalloc(sizeof(pg_tbl1), physfree, 1);
    print("\nAddress : %x", pg_tbl1_i);

    pg_tbl2 *pg_tbl2_i = (pg_tbl2 *)kmalloc(sizeof(pg_tbl2), physfree, 1);
    print("\nAddress : %x", pg_tbl2_i);

    pg_tbl3 *pg_tbl3_i = (pg_tbl3 *)kmalloc(sizeof(pg_tbl3), physfree, 1);
    print("\nAddress : %x", pg_tbl3_i);

    set_pg_dir(pg_dir, 511, (uint64_t)pg_tbl1_i);
    set_pg_tbl1(pg_tbl1_i, 510, (uint64_t)pg_tbl2_i);
    set_pg_tbl2(pg_tbl2_i, 1, (uint64_t)pg_tbl3_i);

    uint64_t physbase_t = 0x200000;
    uint64_t physfree_t = 0x20b000;
    int i =0;
    for (; physbase_t <= physfree_t; physbase_t += 0x1000)
    {
        set_pg_tbl3(pg_tbl3_i, i, physbase_t);
        i++;
    }
    //set_pg_tbl3(&pg_tbl3_i, 0, (uint64_t)0x200000);

    switch_page_directory(pg_dir);
}

void switch_page_directory(page_dir *dir)
{
    //print("\n%x", &dir->pg_dir_ptr[0]);
    load_cr3(dir );


//   current_directory = dir;
 /*   print("\n%x", &dir);
    print("\n%x", &dir->pg_dir_ptr);
    print("\n%x", &dir->pg_dir_ptr[0]);
    print("\n%x", &dir->pg_dir_ptr[511]);*/
    //print("\n%x", ((*dir).pg_dir_ptr)[511]);

  /*  asm volatile("movq %%cr3, %0":: "b"(((uint64_t)&dir->pg_dir_ptr) << 12));
    unsigned int cr0;
    asm volatile("movq %0, %%cr0": "=b"(cr0));
    cr0 |= 0x80000000;
    asm volatile("movq %%cr0, %0":: "b"(cr0));

    unsigned int cr4;
    asm volatile("mov %%cr0, %0": "=b"(cr4));
    cr4 |= 0x5;
    asm volatile("mov %0, %%cr0":: "b"(cr4));
*/
    /*asm volatile ("movl %0, %%cr3" :: "a" (((uint64_t)&dir->pg_dir_ptr) << 12));*/
    //asm volatile ("mov %cr4, %eax; bts $5, %eax; movl %eax, %cr4"); // set bit5 in CR4 to enable PAE
    //asm volatile ("mov %%eax, %%cr3" :: "a" (((uint64_t)&dir->pg_dir_ptr) << 12));
    //asm volatile ("mov %cr0, %eax; orl $0x80000000, %eax; movl %eax, %cr0;");

//    asm volatile("mov %0, %%cr3":: "r"());
//    uint64_t cr0;
//    asm volatile("mov %%cr0, %0": "=r"(cr0));
//    cr0 |= 0x80000000; // Enable paging!
//    asm volatile("mov %0, %%cr0":: "r"(cr0));
}


void set_pg_dir(page_dir *p_dir, uint64_t index, uint64_t value)
{
    uint64_t flags = 0x007;
    uint64_t address = value;
    address = address | flags;
    p_dir->pg_dir_ptr[index] = address;
}

void set_pg_tbl1(pg_tbl1 *pg_tbl, uint64_t index, uint64_t value)
{
    uint64_t flags = 0x007;
    uint64_t address = value;
    address = address | flags;

    pg_tbl->pg_tbl1_ptr[index] = address;
}

void set_pg_tbl2(pg_tbl2 *pg_tbl, uint64_t index, uint64_t value)
{
    uint64_t flags = 0x007;
    uint64_t address = value;
    address = address | flags;

    pg_tbl->pg_tbl2_ptr[index] = address;
}

void set_pg_tbl3(pg_tbl3 *pg_tbl, uint64_t index, uint64_t value)
{
    uint64_t flags = 0x007;
    uint64_t address = value;
    address = address | flags;

    pg_tbl->pg_tbl3_ptr[index] = address;
}

uint64_t kmalloc(uint64_t sz, char * physfree, int align)
{
    uint64_t phys_addr = (uint64_t)physfree;
    while(get_mem_bit(phys_addr/PAGE_SIZE) != AVAILABLE)
    {
        phys_addr += PAGE_SIZE;
    }

    set_mem_bit((uint64_t)phys_addr/PAGE_SIZE);

    uint64_t placement_address = (uint64_t)phys_addr;
    if (align == 1 && (placement_address & (uint64_t)0xFFFFF000)) // If the address is not already page-aligned
    {
        // Align it.
        placement_address &= 0xFFFFF000;
        placement_address += 0x1000;
    }
    uint64_t address = placement_address;

    return address;
}

int get_phy_page()
{
    return 0;
}

void set_mem_bit(uint64_t position)
{
    phy_mem_map[position / 8] |= (1 << (position % 8));
}

int get_mem_bit(uint64_t position)
{
    return (phy_mem_map[position / 8] >>  (position % 8)) & 1;
}

void unset_mem_bit(int position)
{
    phy_mem_map[position / 8] &= ~ (1 << (position % 8));
}

