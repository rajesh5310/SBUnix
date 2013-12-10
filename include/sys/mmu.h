#ifndef MMU_H
#define MMU_H
// page number field of address
#define PPN(pa)		(((uintptr_t) (pa)) >> PTXSHIFT)
#define VPN(la)		PPN(la)		// used to index into vpt[]

// page directory index
#define PDX(la)		((((uintptr_t) (la)) >> PDXSHIFT) & 0x1FF)
#define VPD(la)		(((uintptr_t) (la)) >> PDXSHIFT)		// used to index into vpd[]
#define VPDPE(la)   (((uintptr_t) (la)) >> PDPESHIFT)
#define VPML4E(la)  (((uintptr_t) (la)) >> PML4SHIFT)

#define PML4(la)  ((((uintptr_t) (la)) >> PML4SHIFT) & 0x1FF)

// page table index
#define PTX(la)		((((uintptr_t) (la)) >> PTXSHIFT) & 0x1FF)
#define PDPE(la)   ((((uintptr_t) (la)) >> PDPESHIFT) & 0x1FF)


// offset in page
#define PGOFF(la)	(((uintptr_t) (la)) & 0xFFF)

// construct linear address from indexes and offset
#define PGADDR(m,p,d, t, o)	((void*) ((m) << PML4SHIFT| (p) << PDPESHIFT | (d) << PDXSHIFT | (t) << PTXSHIFT | (o)))

// Page directory and page table constants.
#define NPMLENTRIES	512		// page directory entries per page directory
#define NPDPENTRIES	512		// page table entries per page table
#define NPDENTRIES     512
#define NPTENTRIES     512

#define PGSIZE		4096		// bytes mapped by a page
#define PGSHIFT		12		// log2(PGSIZE)

#define PTSIZE		(PGSIZE*NPTENTRIES) // bytes mapped by a page directory entry
#define PTSHIFT		21		// log2(PTSIZE)

#define PTXSHIFT	12		// offset of PTX in a linear address
#define PDXSHIFT	21		// offset of PDX in a linear address
#define PDPESHIFT    30
#define PML4SHIFT   39

// Page table/directory entry flags.
#define PTE_P		0x001	// Present
#define PTE_W		0x002	// Writeable
#define PTE_U		0x004	// User
#define PTE_PWT		0x008	// Write-Through
#define PTE_PCD		0x010	// Cache-Disable
#define PTE_A		0x020	// Accessed
#define PTE_D		0x040	// Dirty
#define PTE_PS		0x080	// Page Size
#define PTE_MBZ		0x180	// Bits must be zero

// The PTE_AVAIL bits aren't used by the kernel or interpreted by the
// hardware, so user processes are allowed to set them arbitrarily.
#define PTE_AVAIL	0xE00	// Available for software use

// Flags in PTE_SYSCALL may be used only in system calls. (Others may not.)
#define PTE_SYSCALL (PTE_AVAIL | PTE_P | PTE_W | PTE_U)

// Only flags in PTE_USER may be used in system calls.
#define PTE_USER	(PTE_AVAIL | PTE_P | PTE_W | PTE_U)

// Address in page table or page directory entry
#define PTE_ADDR(pte)	((physaddr_t) (pte) & ~0xFFF)

// Page fault error codes
#define FEC_PR		0x1	// Page fault caused by protection violation
#define FEC_WR		0x2	// Page fault caused by a write
#define FEC_U		0x4	// Page fault occured while in user mode

#endif /* !MMU_h */
