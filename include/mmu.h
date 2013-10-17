#ifndef MMU_H
#define MMU_H

/*
 * This file contains definitions for the x86 memory management unit (MMU),
 * including paging- and other data structures and constants,
 * the %cr0, %cr4, and %eflags registers, and traps.
 */

/*
 *
 *	Part 1.  Paging data structures and constants.
 *
 */

// A linear address 'la' has a three-part structure as follows:
//
// +-------9--------+-------9--------+--------9-------+--------9-------+----------12---------+
// |Page Map Level 4|Page Directory  | Page Directory |   Page Table   | Offset within Page  |
// |      Index     |  Pointer Index |      Index     |      Index     |                     |
// +----------------+----------------+----------------+--------------------------------------+
// \----PML4(la)----/\--- PDPE(la)---/\--- PDX(la) --/ \--- PTX(la) --/ \---- PGOFF(la) ----/
//  \------------------------------ VPN(la) -------------------------/
//
// The PML4, PDPE, PDX, PTX, PGOFF, and VPN macros decompose linear addresses as shown.
// To construct a linear address la from PML4(la), PDPE(la), PDX(la), PTX(la), and PGOFF(la),
// use PGADDR(PML4(la), PDPE(la), PDX(la), PTX(la), PGOFF(la)).

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

// Control Register flags
/*
#define CR0_PE		0x00000001	// Protection Enable
#define CR0_MP		0x00000002	// Monitor coProcessor
#define CR0_EM		0x00000004	// Emulation
#define CR0_TS		0x00000008	// Task Switched
#define CR0_ET		0x00000010	// Extension Type
#define CR0_NE		0x00000020	// Numeric Errror
#define CR0_WP		0x00010000	// Write Protect
#define CR0_AM		0x00040000	// Alignment Mask
#define CR0_NW		0x20000000	// Not Writethrough
#define CR0_CD		0x40000000	// Cache Disable
#define CR0_PG		0x80000000	// Paging

#define CR4_PCE		0x00000100	// Performance counter enable
#define CR4_MCE		0x00000040	// Machine Check Enable
#define CR4_PSE		0x00000010	// Page Size Extensions
#define CR4_DE		0x00000008	// Debugging Extensions
#define CR4_TSD		0x00000004	// Time Stamp Disable
#define CR4_PVI		0x00000002	// Protected-Mode Virtual Interrupts
#define CR4_VME		0x00000001	// V86 Mode Extensions

//x86_64 related changes
#define CR4_PAE     0x00000020
#define EFER_MSR    0xC0000080
#define EFER_LME    8

// Eflags register
#define FL_CF		0x00000001	// Carry Flag
#define FL_PF		0x00000004	// Parity Flag
#define FL_AF		0x00000010	// Auxiliary carry Flag
#define FL_ZF		0x00000040	// Zero Flag
#define FL_SF		0x00000080	// Sign Flag
#define FL_TF		0x00000100	// Trap Flag
#define FL_IF		0x00000200	// Interrupt Flag
#define FL_DF		0x00000400	// Direction Flag
#define FL_OF		0x00000800	// Overflow Flag
#define FL_IOPL_MASK	0x00003000	// I/O Privilege Level bitmask
#define FL_IOPL_0	0x00000000	//   IOPL == 0
#define FL_IOPL_1	0x00001000	//   IOPL == 1
#define FL_IOPL_2	0x00002000	//   IOPL == 2
#define FL_IOPL_3	0x00003000	//   IOPL == 3
#define FL_NT		0x00004000	// Nested Task
#define FL_RF		0x00010000	// Resume Flag
#define FL_VM		0x00020000	// Virtual 8086 mode
#define FL_AC		0x00040000	// Alignment Check
#define FL_VIF		0x00080000	// Virtual Interrupt Flag
#define FL_VIP		0x00100000	// Virtual Interrupt Pending
#define FL_ID		0x00200000	// ID flag
*/
// Page fault error codes
#define FEC_PR		0x1	// Page fault caused by protection violation
#define FEC_WR		0x2	// Page fault caused by a write
#define FEC_U		0x4	// Page fault occured while in user mode

#endif /* !MMU_h */
