
#include <sys/pmap.h>
#include <sys/memlayout.h>
#include <sys/ahci.h>
#include <sys/mmu.h>
#include <sys/print.h>
#include <defs.h>
#include <string.h>
#include <sys/mmu.h>
#include <error.h>


uintptr_t end;

void check_page_free_list(bool);
void check_page_alloc(void);

size_t npages;          // Amount of physical memory (in pages)
static size_t npages_basemem;   // Amount of base memory (in pages)

pml4e_t *boot_pml4e;        // Kernel's initial page directory
physaddr_t boot_cr3;        // Physical address of boot time page directory

struct Page *pages;     // Physical page state array
static struct Page *page_free_list; // Free list of physical pages

uint64_t *pte;
uint64_t bar5;

void mm_init(uint32_t* modulep, void* physbase, void* physfree)
{

    pml4e_t* pml4e;

    static size_t npages_extmem;   // Amount of upper memory (in pages)
    end=KERNBASE+(uintptr_t)physfree;

    struct smap_t {
        uint64_t base, length;
        uint32_t type;
    }__attribute__((packed)) *smap;
    while(modulep[0] != 0x9001) modulep += modulep[1]+2;
    for(smap = (struct smap_t*)(modulep+2); smap < (struct smap_t*)((char*)modulep+modulep[1]+2*4); ++smap) {
        if (smap->type == 1 /* memory */ && smap->length != 0) {

            if(smap->base==0) {
                npages_basemem=smap->length/PGSIZE;
                print("\nLower Memory Pages = %d\n",npages_basemem);
            }
            else {
                npages_extmem=((smap->base+smap->length) - (uintptr_t)physfree)/PGSIZE;
                print("\nHigher Memory Pages = %d\n",npages_extmem);
            }

            print("Available Physical Memory [%x-%x]\n", smap->base, smap->base + smap->length);
        }
    }

    npages=npages_basemem+npages_extmem;
    print("\nAvailable Physical Pages [%d]\n", npages);		

    pml4e = boot_alloc(PGSIZE);

    memset(pml4e, 0, PGSIZE);
    boot_pml4e = pml4e;
    boot_cr3 = PADDR(pml4e);

    pages = (struct Page*)boot_alloc(npages*sizeof(struct Page));


    page_init(); 
    //    boot_map_segment( pml4e,MEMBASE,256*1024*1024, (uintptr_t)0x0,PTE_W|PTE_P);
    boot_map_segment(pml4e,KERNBASE+(uintptr_t)physbase, 0x7ffe000, (uintptr_t)physbase,PTE_W|PTE_P|PTE_U);
    boot_map_segment( pml4e,KERNBASE+(uintptr_t)0xb8000, 4096*3, (uintptr_t)0xb8000,PTE_W|PTE_P|PTE_U);
    bar5 = checkAllBuses();
    boot_map_segment( pml4e, 0xFFFFFFFF00000000+(uintptr_t)bar5, 4096, (uintptr_t)bar5,PTE_W|PTE_P|PTE_U);
   asm volatile("mov %0, %%cr3":: "b"(boot_cr3));
    change_video_pointer();


    struct Page *p=page_alloc(0);

    print("\n%x-%x\n",p,PADDR(p));


}


    void *
boot_alloc(uint64_t n)
{
    static char *nextfree;  // virtual address of next byte of free memory
    char *result = 0;
    if (!nextfree) {
        nextfree = ROUNDUP((char *) end, PGSIZE);
        result=nextfree;
        nextfree+=n;
    }
    else
    {
        if(n==0) {
            nextfree = ROUNDUP((char *) nextfree, PGSIZE);          /*Fix 1*/
            PADDR(nextfree);
            result=nextfree;
        }
        else
            if(n>0)
            {
                nextfree = ROUNDUP((char *) nextfree, PGSIZE);
                result=nextfree;
                nextfree+=n;
                PADDR(nextfree);
            }
    }
    return result;

}


    void
page_init(void)
{

    size_t i;
    physaddr_t pa;
    for (i = 0; i < npages; i++) {
        if (i==0) {
            pages[i].pp_ref = 1;
            pages[i].pp_link = NULL;
        } else if (i<npages_basemem) {
            pages[i].pp_ref = 0;
            pages[i].pp_link = page_free_list;
            page_free_list = &pages[i];
        } else if ((i<=(EXTPHYSMEM / PGSIZE))
                || (i<(((uint64_t)boot_alloc(0) - KERNBASE)>>PGSHIFT))){
            pages[i].pp_ref++;
            pages[i].pp_link = NULL;
        } else {
            pages[i].pp_ref = 0;
            pages[i].pp_link = page_free_list;
            page_free_list = &pages[i];
        }

        pa = page2pa(&pages[i]);

        if ((pa == 0 || pa == IOPHYSMEM)
                && (pages[i].pp_ref==0))
            print("page error: i %d\n", i);
    } 

}
    struct Page *
page_alloc(int alloc_flags)
{

    struct Page * pp = NULL;

    if (!page_free_list)
        return NULL;

    pp = page_free_list;

    page_free_list = page_free_list->pp_link;

    if (alloc_flags & ALLOC_ZERO)
        memset(page2kva(pp), 0, PGSIZE);
    return pp;

}


//
// Return a page to the free list.
// (This function should only be called when pp->pp_ref reaches 0.)
//
    void
page_free(struct Page *pp)
{
    assert(pp->pp_ref==0);
    pp->pp_link = page_free_list;
    page_free_list = pp;
}



//
// Decrement the reference count on a page,
// freeing it if there are no more refs.
//
    void
page_decref(struct Page* pp)
{
    if (--pp->pp_ref == 0)
        page_free(pp);
}




    pte_t *
pml4e_walk(pml4e_t *pml4e, const void *va, int create)
{
    pdpe_t        *pdpe_ptr;
    pte_t         *pte_ptr;

    struct Page   *pp=NULL;

    if (!pml4e[PML4(va)] & PTE_P) {
        switch (create) {
            case 0 :
#if DEBUG
                print("PWALK : |PLM4 No Entry Created| : [index] %d\n",PML4(va));
#endif

                return NULL;
            case 1 :
                if((pp = page_alloc(ALLOC_ZERO))!=NULL) {
#if DEBUG                          
                    print("PWALK : |PLM4 Entry Created| : [index] %d\n",PML4(va));
#endif

                    *(pml4e+PML4(va)) = (pml4e_t)(page2pa(pp)) | PTE_P | PTE_U | PTE_W;
                    // *(pml4e+PML4(va)) = (pml4e_t)(page2pa(pp)) | PTE_P  | PTE_W;
                    pp->pp_ref++;

                }
                else {
#if DEBUG
                    print("PWALK : |PML4E Index Allocation Failed|\n");
#endif

                    return NULL;
                }

                break;

            default :
                break;

        }
    }

    //      cprintf (" DBG :: PML4E :%x\n",pml4e);
    //      cprintf (" DBG :: PML4E[PML4(va)] :%x\n",pml4e[PML4(va)]);

    pdpe_ptr = KADDR(PTE_ADDR(pml4e[PML4(va)]));

#if DEBUG
    print("PWALK : pml4[%d]=%x->%x\n",PML4(va),pml4e[PML4(va)],pdpe_ptr);
#endif

    if((pte_ptr = pdpe_walk(pdpe_ptr, va, create))!=NULL)
        return pte_ptr;
    else {
        if(pp!=NULL) {
            pml4e[PML4(va)] = 0;
            page_decref(pp);
        }
        return NULL;
    }

}


pte_t *
pdpe_walk(pdpe_t *pdpe,const void *va,int create){

    pde_t         *pde_ptr;
    pte_t         *pte_ptr;

    struct Page   *pp = NULL;

    if ((!pdpe[PDPE(va)]) & PTE_P ){
        switch (create) {
            case 0 :
#if DEBUG
                print("PWALK : |PDPE No Entry Created| : [index] %d\n",PDPE(va));
#endif
                return NULL;
            case 1 :
                if((pp = page_alloc(ALLOC_ZERO))!=NULL) {
#if DEBUG
                    print("PWALK : |PDPE Entry Created| : [index] %d\n",PDPE(va));
#endif

                    *(pdpe+PDPE(va)) = (pdpe_t)page2pa(pp) | PTE_P | PTE_U | PTE_W;
                    //  *(pdpe+PDPE(va)) = (pdpe_t)page2pa(pp) | PTE_P | PTE_W;
                    pp->pp_ref++;
                }

                else {
#if DEBUG
                    print("PWALK : |PDPE Index Allocation Failed|\n");
#endif

                    return NULL;
                }

                break;
            default :
                break;

        }

    }

    //cprintf("J_PWALK :pdpe at %x  pdpe[%d]=%x\n",pdpe, PDPE(va),pdpe[PDPE(va)]);

    pde_ptr = KADDR(PTE_ADDR(pdpe[PDPE(va)]));

#if DEBUG

    print("PWALK : pdpe[%d]=%x->%x\n",PDPE(va),pdpe[PDPE(va)],pde_ptr);

#endif

    if((pte_ptr = pgdir_walk(pde_ptr, va, create))!=NULL)
        return pte_ptr;
    else {
        if(pp != NULL) {
            pdpe[PDPE(va)] = 0;
            page_decref(pp);
        }
        return NULL;
    }
}

    pte_t *
pgdir_walk(pde_t *pgdir, const void *va, int create)
{
    pte_t  *pte_ptr;
    struct Page   *pp;

    if (!pgdir[PDX(va)] & PTE_P) {
        switch (create) {
            case 0 :
#if DEBUG
                print("PWALK : |PDE No Entry Created| : [index] %d\n",PDX(va));
#endif
                return NULL;
            case 1 :

                if((pp = page_alloc(ALLOC_ZERO))!=NULL) {
                    //cprintf("J_PWALK : |PDE Entry Created| : [index] %d\n",PDX(va));
                    pgdir[PDX(va)] = (pde_t)page2pa(pp) | PTE_P | PTE_W | PTE_U;
                    //  pgdir[PDX(va)] = (pde_t)page2pa(pp) | PTE_P | PTE_W ;

                    if(PDX(va) >= PDX(KERNBASE))
                        pgdir[PDX(va)] = (pde_t)page2pa(pp) | PTE_P | PTE_W | PTE_U; /* Fix : during Lab 3 for Ex 5*/
                    //   pgdir[PDX(va)] = (pde_t)page2pa(pp) | PTE_P | PTE_W; /* Fix : during Lab 3 for Ex 5*/

                    pp->pp_ref++;

                }
                else {
#if DEBUG
                    print("PWALK : |PDE Index Allocation Failed|\n");
#endif
                    return NULL;
                }
                break;
        }

    }

    //      cprintf("J_PWALK : pdpe[%d]  pde[%d]=%x\n",PDPE(va), PDX(va),pgdir[PDX(va)]);

    pte_ptr = KADDR(PTE_ADDR(*(pgdir+PDX(va))));

#if DEBUG
    print("PWALK : pde[%d]=%x->%x\n",PDX(va),pgdir[PDX(va)],pte_ptr);
#endif

    pte_ptr = &pte_ptr[PTX(va)];

    return   pte_ptr;

}



    void
boot_map_segment(pml4e_t *pml4e, uintptr_t la, size_t size, physaddr_t pa, int perm)
{
    // Fill this function in
    uint32_t i  =0;
    pte_t   *pte_ptr;

    uintptr_t va = ROUNDUP(la,PGSIZE);

    for (i = 0; i < ROUNDUP(size, PGSIZE); i+=PGSIZE) {             /*Lab4 Fix */
        pte_ptr = pml4e_walk(pml4e, (char*)(va+i), 1);

        if(pte_ptr!=NULL) {
            *pte_ptr = pa+i;
            *pte_ptr = *pte_ptr | (perm | PTE_P | PTE_U | PTE_W) ;
            //  *pte_ptr = *pte_ptr | (perm | PTE_P) ;
        }

        else
            print(" Null Boot map segment\n");

#if DEBUG
        print("P_INSERT : : PDP :%d [va] %x -> [PTE Entry]:%x\n",PDX(la), la+i,PTE_ADDR(*pte_ptr));
#endif     
    }
}


/*
   bump allocator for user process   
 */

uint64_t *kmalloc(uint64_t size)
{
    //uint32_t temp_size = size;
    uint64_t temp_size = size;
    uint64_t pages_required = 0;

    struct Page* pp = NULL;

    //uint64_t *start_address = NULL;
    int i = 0;

    // Calculating the no of pages required corresponding to the size
    if (temp_size < PGSIZE)
        pages_required = 1;
    else {
        pages_required = temp_size/PGSIZE;
        temp_size -= (pages_required*PGSIZE);

        if (temp_size >0) 
            pages_required++;
    }   

    // Getting the pages allocated  
    if (!page_free_list)
        return NULL;

    pp = page_free_list;
    uint64_t *start_address =(uint64_t *)page2kva(pp); 

    for(i = 0; i< pages_required; i++) {
        pp->pp_ref++;
        memset(page2kva((void*)pp), 0, PGSIZE);
        pp = pp->pp_link;
    }   

    page_free_list = pp; 
    //    print("Starting address for %d pages is %x ", pages_required, start_address);

    return start_address;
}


uint64_t * kmalloc_user (pml4e_t *pml4e,uint64_t size) {
    static uint64_t *bumpPtr=(uint64_t *)0xFFFFF0F080700000;     // start vm from here 
    uint64_t *ret;
    ret = bumpPtr; 
    uint64_t no_of_blocks;
    uint64_t *pte=NULL;
    uint64_t i;

    struct Page *pp=NULL;

    no_of_blocks = size/PGSIZE;

    if((size - (size/PGSIZE)*PGSIZE) != 0)
        no_of_blocks ++ ;

    //  printf("1: %p\n",bumpPtr);
    //  printf("blocks %d\n",no_of_blocks);

    while(no_of_blocks) {
        no_of_blocks--;
        for (i = 0; i < ROUNDUP(size, PGSIZE);  i+=PGSIZE) {             /*Lab4 Fix */
            pte = pml4e_walk(pml4e, (char*)bumpPtr, 1); 

            pp = page_alloc(ALLOC_ZERO);
            pp->pp_ref++;

            *pte = ((uint64_t)page2pa(pp)) | PTE_P | PTE_W | PTE_U;
            print("%x-%x",pte,*pte);
        }                                      
        bumpPtr += 512; 
    }   

    return ret;

}


    int
page_insert(pml4e_t *pml4e, struct Page *pp, void *va, int perm)
{
    pte_t * pte = pml4e_walk(pml4e, (void*)va, 1);
    if (pte == NULL) return -E_NO_MEM;

    pp->pp_ref++;
    if(*pte & PTE_P)
        page_remove(pml4e, va);
    *pte = ((uint64_t)page2pa(pp)) | perm | PTE_P | PTE_U | PTE_W;

    return 0;
}

    struct Page *
page_lookup(pml4e_t *pml4e, void *va, pte_t **pte_store)
{
    pte_t * pte = pml4e_walk(pml4e, (void*)va, 0);
    if (pte == NULL) {
        *pte_store = NULL;
        return NULL;
    }

    if (*pte != 0) {
        if (pte_store != NULL)
            *pte_store = pte;
        return pa2page((physaddr_t)(PTE_ADDR(*pte)));
    }

    return NULL;
}



static inline void __native_flush_tlb_single(uint64_t addr)
{
    asm volatile("invlpg (%0)" ::"r" (addr) : "memory");

}


//
// Invalidate a TLB entry, but only if the page tables being
// edited are the ones currently in use by the processor.
//
    void
tlb_invalidate(pml4e_t *pml4e, void *va)
{
    //        assert(pml4e != NULL);
    // Flush the entry only if we're modifying the current address space.
    //        assert(pml4e!=NULL);
    //        if (!curenv || curenv->env_pml4e == pml4e)
    __native_flush_tlb_single((uint64_t)va);
}


   void
page_remove(pml4e_t *pml4e, void *va)
{
    pte_t *pte;
    struct Page *pp = page_lookup(pml4e, va, &pte);
    if (pp) {
        if (pte) {
            *pte = 0;
            tlb_invalidate(pml4e, va);
        }
        page_decref(pp);
    }
}
uint64_t kmalloc_ahci(uint64_t size)
{
        uint32_t temp_size = size;
        int pages_required = 0;
        struct Page* pp = NULL;
        uint64_t start_address;
        int i = 0;
        // Calculating the no of pages required corresponding to the size
        if (temp_size < PGSIZE)
                pages_required = 1;
        else
                {
                pages_required = temp_size/PGSIZE;
                temp_size -= (pages_required*PGSIZE);
                if (temp_size >0)
                        pages_required++;
                }
        // Getting the pages allocated  
        if (!page_free_list)
                return 0;

        pp = page_free_list;
        start_address =(uint64_t) pp;

        for(i = 0; i< pages_required; i++)
        {
                pp->pp_ref++;
                memset(page2kva((void*)pp), 0, PGSIZE);
                pp = pp->pp_link;
        }
        page_free_list = pp;
        print("Starting address for %d pages is %x ", pages_required, start_address);
        return start_address;
}

