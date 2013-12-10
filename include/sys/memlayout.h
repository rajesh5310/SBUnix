#ifndef MEMLAYOUT_H
#define MEMLAYOUT_H

#include <defs.h>
#include <sys/mmu.h>
// All physical memory mapped at this address
#define	KERNBASE	0xffffffff80000000

// at physical address EXTPHYSMEM.
#define IOPHYSMEM	0x09fc00
#define EXTPHYSMEM	0x100000


#define ULIM	    (KERNBASE-PGSIZE)
#define USTACKTOP	ULIM
#define USTACK	    0xFFFFF0F080700000


// User read-only virtual page table (see 'vpt' below)

#define UVPT    0x10000000000
// Read-only copies of the Page structures
#define UPAGES		(ULIM - PTSIZE)
// Read-only copies of the global env structures
#define UENVS		(UPAGES - PTSIZE)

/*
 * Top of user VM. User can manipulate VA from UTOP-1 and down!
 */

// Top of user-accessible VM
#define UTOP		UENVS

// Where user programs generally begin
#define UTEXT		(4*PTSIZE)

// Used for temporary page mappings.  Typed 'void*' for convenience

#define UTEMP		((void*) ((int)(2*PTSIZE)))
// Used for temporary page mappings for the user page-fault handler
// (should not conflict with other temporary page mappings)
#define PFTEMP		(UTEMP + PTSIZE - PGSIZE)
// The location of the user-level STABS data structure
#define USTABDATA	(PTSIZE)

// Physical address of startup code for non-boot CPUs (APs)
#define MPENTRY_PADDR	0x7000


typedef uint64_t pml4e_t;
typedef uint64_t pdpe_t;
typedef uint64_t pte_t;
typedef uint64_t pde_t;
extern volatile pte_t vpt[];     // VA of "virtual page table"
extern volatile pde_t vpd[];     // VA of current page directory
extern volatile pde_t vpde[];     // VA of current page directory pointer
extern volatile pde_t vpml4e[];     // VA of current page map level 4
struct Page {
    // Next page on the free list.
    struct Page *pp_link;

    // pp_ref is the count of pointers (usually in page table entries)
    // to this page, for pages allocated using page_alloc.
    // Pages allocated at boot time using pmap.c's
    // boot_alloc do not have valid reference count fields.

    uint16_t pp_ref;
};

#endif /* !MEMLAYOUT_H */
