#include<sys/vmem.h>
#include<print.h>
#include<defs.h>
void setup_page_tables();
char phy_mem_map[NUMBER_OF_PAGES/8]; //1 bit for each 8 bits ; 1 character takes 8 bits and can hence store 8 pages.
uint64_t * placement_addr;
uint64_t kmalloc(uint64_t sz, uint64_t placemenet_addr, int align);
void init_phy_mem(uint32_t* modulep, void* physbase, void* physfree)
{
    placement_addr = physfree;

    uint64_t total_phy_mem = 0, avail_phy_mem = 0, length=0;
    uint64_t i = 0;
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
//   kprintf("\n====> physfree:%p and physbase:%p",physfree, physbase);
//   kprintf("\n====> Phy_memory_map address:%p",&phy_mem_map);

   memset(phy_mem_map, 0x00, sizeof(phy_mem_map)); //Set All bits Available

   // SET BITS UNAVAILABLE FOR the pages corresponding from physbaffffffff80205018se to physfree
  for (i = (uint64_t) physbase / PAGE_SIZE; i < ((uint64_t)physfree /PAGE_SIZE)+ 100; i++)
      set_mem_bit(i);

    //the page directory comes right after the kernel - NOTE: make sure the address is page aligned!
   // uint64_t page_aligned_end = ((uint64_t)physfree%PAGE_SIZE);
    uint64_t *page_directory = (uint64_t *)((uint64_t)physfree);



    print("\n%p", page_directory);

    setup_page_tables();
}
void setup_page_tables()
{
    uint64_t address = (uint64_t)placement_addr;
    print("<-----------ADDR---------------> %x", address);
    page_dir *pg_dir = (page_dir *)kmalloc(sizeof(page_dir), address, 1);
    print("\nAddress : %x", &pg_dir);
    address += 4098;
    pg_tbl1 *pg_tbl1_1 = (pg_tbl1 *)kmalloc(sizeof(pg_tbl1), address, 1);
    print("\nAddress : %x", &pg_tbl1_1);
    address += 4098;
    pg_tbl2 *pg_tbl2_1_5 = (pg_tbl2 *)kmalloc(sizeof(pg_tbl2), address, 1);
    print("\nAddress : %x", &pg_tbl2_1_5);
    address += 4098;
    pg_tbl3 *pg_tbl3_1_5_10 = (pg_tbl3 *)kmalloc(sizeof(pg_tbl3), address, 1);
    print("\nAddress : %x", &pg_tbl3_1_5_10);
    address += 4098;

    set_pg_dir(&pg_dir, 0, (uint64_t)&pg_tbl1_1);
    set_pg_tbl1(&pg_tbl1_1, 5, (uint64_t)&pg_tbl2_1_5);
    set_pg_tbl2(&pg_tbl2_1_5, 10, (uint64_t)&pg_tbl3_1_5_10);
    set_pg_tbl3(&pg_tbl3_1_5_10, 15, (uint64_t)0x200000);
}

void set_pg_dir(page_dir **p_dir, uint64_t index, uint64_t value)
{
    uint64_t flags = 0x001;
    uint64_t address = value;
    address = address << 12;
    address = address | flags;
    ((*p_dir)->pg_dir_ptr)[index] = address;
    print("\n%x", address);
}

void set_pg_tbl1(pg_tbl1 **pg_tbl, uint64_t index, uint64_t value)
{
    uint64_t flags = 0x001;
    uint64_t address = value;
    address = address << 12;
    address = address | flags;

    ((*pg_tbl)->pg_tbl1_ptr)[index] = address;
    print("\n%x", address);
}

void set_pg_tbl2(pg_tbl2 **pg_tbl, uint64_t index, uint64_t value)
{
    uint64_t flags = 0x001;
    uint64_t address = value;
    address = address << 12;
    address = address | flags;

    ((*pg_tbl)->pg_tbl2_ptr)[index] = address;
    print("\n%x", address);
}

void set_pg_tbl3(pg_tbl3 **pg_tbl, uint64_t index, uint64_t value)
{
    uint64_t flags = 0x001;
    uint64_t address = value;
    address = address << 12;
    address = address | flags;

    ((*pg_tbl)->pg_tbl3_ptr)[index] = address;

    /*if((address)%PAGE_SIZE_HEX != 0)
    {
        print("Memory address not page aligned!");
        return;
    }*/
    uint64_t page_number = (value)/PAGE_SIZE_HEX + 1;
    set_mem_bit(page_number);

    print("\nAddress : %x", address);
}

uint64_t kmalloc(uint64_t sz, uint64_t placement_address, int align)
{
  /*if (align == 1 && (placement_address & (uint64_t)0xFFFFF000)) // If the address is not already page-aligned
  {
    // Align it.
    placement_address &= 0xFFFFF000;
    placement_address += 0x1000;
  }*/
  uint64_t address = placement_address;
  print("\nsize : %d", sz);
  print("\nplacement address : %x", placement_address);
  //*placement_address += sz;
  return address;
}

int get_phy_page()
{
return 0;
}


void set_mem_bit(int position)
{
  phy_mem_map[position / 8] |= (1 << (position % 8));
}

int get_mem_bit(int position)
{
  return phy_mem_map[position / 8] &  (1 << (position % 8));
}

void unset_mem_bit(int position)
{
  phy_mem_map[position / 8] &= ~ (1 << (position % 8));
}

